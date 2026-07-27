#include "Engine/Core/pch.h"
#include "Scene.h"

#include <Engine/Game/GameObject.h>

namespace engine
{
	Scene::Scene(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name)
	{}
	Scene::~Scene()
	{}
	void Scene::Init()
	{
		has_initialized_ = true;
	}
	void Scene::FrameStart()
	{
		game_objects_.insert(
			game_objects_.end(),
			pending_add_objects_.begin(),
			pending_add_objects_.end()
		);

		pending_add_objects_.clear();

		for (const auto& obj : game_objects_)
		{
			obj->FrameStart();
		}
	}

	void Scene::Update()
	{
		for (const auto& obj : game_objects_)
		{
			if(obj->IsActive()) 
			{ 
				obj->Update();
			}
		}
	}

	void Scene::LateUpdate()
	{
		for (const auto& obj : game_objects_)
		{
			if (obj->IsActive())
			{
				obj->LateUpdate();
			}
		}
	}

	void Scene::FrameEnd()
	{
		for (const auto& obj : game_objects_)
		{
			if (obj)
			{
				//Object에서 자체적으로 Destroy된 컴포넌트들을 제거함
				obj->FrameEnd();
			}
		}

		//Destroy된 GameObject들 제거
		std::erase_if(
			game_objects_,
			[](const s_ptr<GameObject>& obj) { return obj->IsDestroyed(); }
		);
	}

	void Scene::AddGameObject(s_ptr<GameObject> obj)
	{
		if (obj)
		{
			obj->SetOwnerScene(std::static_pointer_cast<Scene>(shared_from_this()));
			if (!(obj->HasInitialized()))
			{
				obj->Init();
			}
			pending_add_objects_.push_back(obj);
		}
	}

	s_ptr<GameObject> Scene::AddGameObject(const HashedStringView& concrete_class_name)
	{
		auto obj = EntityFactory::GetInst().CreateEntityAs<GameObject>(concrete_class_name);
		if (obj)
		{
			AddGameObject(obj);
		}
		return obj;
	}
}