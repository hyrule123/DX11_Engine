#include "Engine/Core/pch.h"
#include "SceneManager.h"

#include <Engine/Game/Scene.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::ChangeScene(u_ptr<Scene> scene)
	{
		next_scene_ = std::move(scene);

		if (next_scene_)
		{
			if (!cur_scene_)
			{
				ChangeSceneNow();
			}
		}
	}
	void SceneManager::ChangeScene(const HashedStringView& concrete_class_name)
	{
		u_ptr<Scene> scene = EntityManager::GetInst().CreateEntityAs<Scene>(concrete_class_name);
		ASSERT(scene);
		ChangeScene(std::move(scene));
	}
	void SceneManager::Init()
	{
	}

	void SceneManager::FrameStart()
	{
		if (cur_scene_)
		{
			cur_scene_->FrameStart();
		}
	}

	void SceneManager::Update()
	{
		if (cur_scene_)
		{
			cur_scene_->Update();
		}
	}
	void SceneManager::FixedUpdate()
	{
		if (cur_scene_)
		{
			cur_scene_->FixedUpdate();
		}
	}
	void SceneManager::LateUpdate()
	{
		if (cur_scene_)
		{
			cur_scene_->LateUpdate();
		}
	}

	void SceneManager::FrameEnd()
	{
		if (cur_scene_)
		{
			cur_scene_->FrameEnd();
		}
	}

	void SceneManager::ChangeSceneNow()
	{
		if (next_scene_)
		{
			cur_scene_ = std::move(next_scene_);
			next_scene_ = nullptr;
		}
	}
}