#include "Engine/Core/pch.h"
#include "GameEngine.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/SceneManager.h>
#include <Engine/Manager/TimeManager.h>
#include <Engine/Manager/InputManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	GameEngine::GameEngine()
	{
	}
	GameEngine::~GameEngine()
	{
	}

	void GameEngine::OnScreenSizeChange(uint32 width, uint32 height)
	{
		GraphicsDevice::GetInst().OnScreenSizeChange(width, height);
	}

	bool GameEngine::Init()
	{
		if (GraphicsDevice::GetInst().Init() == false)
		{
			ASSERT_RELEASE(false);
			return false;
		}
		ResourceManager::GetInst().Init();
		ResourceManager::GetInst().LoadDefaultResources();

		RenderManager::GetInst().Init();

		SceneManager::GetInst().Init();

		InputManager::GetInst();

		is_initialized_ = true;
		return true;
	}

	bool GameEngine::Run()
	{
		TimeManager::GetInst().Update();

		SceneManager& scene_mgr = SceneManager::GetInst();
		scene_mgr.FrameStart();

		InputManager::GetInst().Update();

		scene_mgr.Update();

		scene_mgr.LateUpdate();

		GraphicsDevice::GetInst().BindSwapChainRTV();
		
		RenderManager::GetInst().Render();

		GraphicsDevice::GetInst().Present();

		RenderManager::GetInst().FrameEnd();
		GraphicsDevice::GetInst().FrameEnd();

		scene_mgr.ChangeSceneNow();
		return true;
	}
}

