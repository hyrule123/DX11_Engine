#include "Engine/Core/pch.h"
#include "GameEngine.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/SceneManager.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	GameEngine::GameEngine()
	{
	}
	GameEngine::~GameEngine()
	{
	}

	bool GameEngine::Init()
	{
		if (GraphicsDevice::GetInst().Init() == false)
		{
			ASSERT_RELEASE(false);
			return false;
		}

		ResourceManager::GetInst().Init();

		SceneManager::GetInst().Init();


		return true;
	}

	bool GameEngine::Run()
	{
		TimeManager::GetInst().Update();

		SceneManager& scene_mgr = SceneManager::GetInst();
		scene_mgr.FrameStart();

		scene_mgr.Update();

		scene_mgr.LateUpdate();

		GraphicsDevice::GetInst().BindSwapChainRTV();
		scene_mgr.Render();

		GraphicsDevice::GetInst().Render();

		scene_mgr.ChangeSceneNow();
		return true;
	}
}

