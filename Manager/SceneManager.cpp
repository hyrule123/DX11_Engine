#include "Engine/pch.h"
#include "SceneManager.h"

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
		cur_scene_ = std::make_shared<DefaultScene>();

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
}