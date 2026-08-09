#include "Engine/Core/pch.h"
#include "Scene.h"

#include <Engine/Game/GameObject.h>

#include <Engine/Core/Debug.h>

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
		FlushPending();
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

		FlushPending();
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
		FlushPending();

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

		FlushPending();
	}

	void Scene::FrameEnd()
	{
		size_t snapshot_size = game_objects_.size();	// Snapshot
		for (size_t i = 0; i < snapshot_size; ++i)
		{
			if (game_objects_[i])
			{
				//Object에서 자체적으로 Destroy된 컴포넌트들을 제거함
				game_objects_[i]->FrameEnd();
			}
		}

		//Destroy된 GameObject들 제거
		std::erase_if(
			game_objects_,
			[](const u_ptr<GameObject>& obj) { return obj->IsDestroyed(); }
		);
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

	void Scene::FlushPending()
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