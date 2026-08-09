#include "Engine/Core/pch.h"
#include "GameEngine.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/SceneManager.h>
#include <Engine/Manager/TimeManager.h>
#include <Engine/Manager/InputManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Core/HandlePointer.h>

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
		TimeManager::GetInst().Init();

		HandlePointerPool::GetInst();	//싱글턴 소멸 시점 고정

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

		SceneManager::GetInst().FrameStart();

		InputManager::GetInst().Update();

#pragma region FIXED_UPDATE
		acc_delta_time_ += TimeManager::GetInst().GetDeltaTime();
		TimeManager::GetInst().SetFixedUpdateMode(true);
		const float fixed_delta_time_ = TimeManager::GetInst().GetFixedDeltaTime();
		uint32 step_count = 0u;
		while (acc_delta_time_ >= fixed_delta_time_)
		{
			SceneManager::GetInst().FixedUpdate();

			acc_delta_time_ -= fixed_delta_time_;

			step_count++;
			//지정된 Step 횟수를 초과 시 업데이트 포기
			if (step_count > max_step_count_)
			{
				acc_delta_time_ = std::fmod(acc_delta_time_, fixed_delta_time_);
				break;
			}
		}
		TimeManager::GetInst().SetFixedUpdateMode(false);
#pragma endregion FIXED_UPDATE

		SceneManager::GetInst().Update();

		SceneManager::GetInst().LateUpdate();

		GraphicsDevice::GetInst().BindSwapChainRTV();
		
		RenderManager::GetInst().Render();

		GraphicsDevice::GetInst().Present();

		RenderManager::GetInst().FrameEnd();
		GraphicsDevice::GetInst().FrameEnd();

		SceneManager::GetInst().FrameEnd();

		SceneManager::GetInst().ChangeSceneNow();
		return true;
	}
}

