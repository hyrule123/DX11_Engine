#include "Engine/Core/pch.h"
#include "SceneManager.h"

#include <Engine/Game/Scene.h>
#include <Engine/Game/DefaultScene.h>

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
		if (next_scene_) { next_scene_->Init(); }
	}
	bool SceneManager::Init()
	{
		ChangeScene(std::make_shared<DefaultScene>());

		return true;
	}
	void SceneManager::Update()
	{
		if (next_scene_)
		{
			cur_scene_ = next_scene_;
			next_scene_ = nullptr;
		}

		if (cur_scene_)
		{
			cur_scene_->Update();
		}
	}
	void SceneManager::Render()
	{
		if (cur_scene_)
		{
			cur_scene_->Render();
		}
	}
}