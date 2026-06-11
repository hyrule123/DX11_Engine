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

	bool SceneManager::Init()
	{
		ChangeScene(std::make_shared<DefaultScene>());

		return true;
	}
	void SceneManager::Update()
	{
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
	void SceneManager::ChangeScene()
	{
		if (next_scene_)
		{
			next_scene_->Init();
			cur_scene_ = next_scene_;
			next_scene_ = nullptr;
		}
	}
}