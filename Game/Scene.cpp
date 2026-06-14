#include "Engine/Core/pch.h"
#include "Scene.h"

#include <Engine/Game/GameObject.h>

namespace engine
{
	Scene::Scene(const std::string_view concrete_class_name)
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
			if (obj)
			{
				obj->FrameStart();
			}
		}
	}

	void Scene::Update()
	{
		for (const auto& obj : game_objects_)
		{
			if(obj) 
			{ 
				obj->Update();
			}
		}
	}

	void Scene::Render()
	{
		for (const auto& obj : game_objects_)
		{
			if (obj)
			{
				obj->Render();
			}
		}
	}

	void Scene::FrameEnd()
	{
		for (const auto& obj : game_objects_)
		{
			if (obj)
			{
				obj->FrameEnd();
			}
		}
	}

	void Scene::AddGameObject(s_ptr<GameObject> obj)
	{
		if (obj)
		{
			obj->SetOwner(std::static_pointer_cast<Scene>(shared_from_this()));
			pending_add_objects_.push_back(obj);
		}
	}
}