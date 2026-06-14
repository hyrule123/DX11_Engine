#include "Engine/Core/pch.h"
#include "SceneManager.h"

#include <Engine/Game/Scene.h>

namespace engine
{
	SceneManager::SceneManager()
	{

	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::ChangeScene(s_ptr<Scene> scene)
	{
		next_scene_ = scene;

		if (next_scene_)
		{
			if (!next_scene_->HasInitialized())
			{
				next_scene_->Init();
			}

			if (!cur_scene_)
			{
				ChangeSceneNow();
			}
		}
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
	void SceneManager::LateUpdate()
	{
		if (cur_scene_)
		{
			cur_scene_->LateUpdate();
		}
	}
	void SceneManager::Render()
	{
		if (cur_scene_)
		{
			cur_scene_->Render();
		}
	}
	void SceneManager::ChangeSceneNow()
	{
		if (next_scene_)
		{
			cur_scene_ = next_scene_;
			next_scene_ = nullptr;
		}
	}
}