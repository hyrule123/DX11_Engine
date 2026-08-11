#include "Engine/Core/pch.h"
#include "Scene.h"

#include <Engine/Game/GameObject.h>

#include <Engine/Core/Debug.h>
#include <Engine/Core/Constant.h>

namespace engine
{
	Scene::Scene(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name)
		, collision_system_2D_(this)
	{}
	Scene::~Scene()
	{}
	void Scene::Init()
	{
		Super::Init();

		collision_system_2D_.Init();
	}

	void Scene::FrameStart()
	{
		FlushPendingAdd();
	}

	void Scene::Update()
	{
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if(game_objects_[i]->IsActive())
			{ 
				game_objects_[i]->Update();
			}
		}

		FlushPendingAdd();
	}

	void Scene::FixedUpdate()
	{
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (game_objects_[i]->IsActive())
			{
				game_objects_[i]->FixedUpdate();
			}
		}
		FlushPendingAdd();

		collision_system_2D_.FixedUpdate();
	}

	void Scene::LateUpdate()
	{
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (game_objects_[i]->IsActive())
			{
				game_objects_[i]->LateUpdate();
			}
		}

		FlushPendingAdd();
	}

	void Scene::FrameEnd()
	{
		// Phase 1: Destroy 되지 않은 GameObject들의 FrameEnd 호출
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			ASSERT(game_objects_[i] != nullptr);
			if(false == game_objects_[i]->IsDestroyed())
			{
				game_objects_[i]->FrameEnd(graveyard_[graveyard_toggle_]);
			}
		}

		// Phase 2: Destroy된 GameObject들의 OnDestroy 호출 및 Graveyard 이동
		// 연쇄적으로 꼬리를 물고 Destroy되는 GameObject가 있을 수 있으므로, 최대 kMaxDestroyPassCount번 반복
		uint32 pass_count = 0;
		for (; pass_count <= kMaxDrainLoopCount; ++pass_count)
		{
			bool any_destroyed = false;

			size_t snapshot_size = game_objects_.size();	// Snapshot
			for (size_t i = 0; i < snapshot_size; ++i)
			{
				if (game_objects_[i] == nullptr || false == game_objects_[i]->IsDestroyed()) 
				{
					continue; 
				}

				//GameObject 자체가 Destroy되었으면 OnDestroy 호출 후, graveyard로 GameObject를 이동
				game_objects_[i]->OnDestroy();
				graveyard_[graveyard_toggle_].push_back(std::move(game_objects_[i]));
				any_destroyed = true;
			}

			// pass에서 Destroy된 GameObject가 없으면 반복 종료
			if (false == any_destroyed)
			{
				break;
			}
		}
		// 최대 pass 도달 시 확인 필요(debug)
		ASSERT_MESSAGE(pass_count < kMaxDrainLoopCount, "Destroy Pass Count exceeded. Possible infinite loop in destruction.");

		//nullptr인 항목들 제거
		std::erase_if(
			game_objects_,
			[](const u_ptr<GameObject>& obj) { return obj == nullptr; }
		);

		// 반대쪽 Graveyard에 있는 SceneEntity들을 모두 제거
		graveyard_[!graveyard_toggle_].clear();

		// 토글 변경
		graveyard_toggle_ = !graveyard_toggle_;
	}

	GameObject* Scene::AddGameObject(const HashedStringView& concrete_class_name)
	{
		return AddGameObject(EntityManager::GetInst().CreateEntityAs<GameObject>(concrete_class_name));
	}
	GameObject* Scene::FindGameObject(const std::string_view name) const
	{
		for (const auto& obj : game_objects_)
		{
			if (obj->GetName() == name)
			{
				return obj.get();
			}
		}
		return nullptr;
	}
	void Scene::SetCollisionMask(uint32 layer_a, uint32 layer_b, bool can_collide)
	{
		ASSERT_MESSAGE(layer_a < kMaxLayers && layer_b < kMaxLayers, "Layer index out of bounds");

		collision_mask_[layer_a][layer_b] = can_collide;
		collision_mask_[layer_b][layer_a] = can_collide; // Ensure symmetry
	}

	GameObject* Scene::AddGameObject(u_ptr<GameObject> obj)
	{
		GameObject* raw_ptr = obj.get();
		if (obj)
		{
			obj->SetOwnerScene(this);
			game_objects_.push_back(std::move(obj));
		}
		return raw_ptr;
	}

	void Scene::FlushPendingAdd()
	{
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (game_objects_[i]->HasPendingComponents())
			{
				game_objects_[i]->FlushPendingComponents();
			}
		}
	}
}