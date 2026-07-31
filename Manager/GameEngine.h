#pragma once

#include <Engine/Core/Singleton.h>
#include <Engine/Core/StdType.h>
#include <Engine/Core/Constant.h>

namespace engine
{
	class GameEngine
	{
		DECLARE_SINGLETON(GameEngine)

		friend class EngineMain;

	public:
		void OnScreenSizeChange(uint32 width, uint32 height);
		bool IsInitialized() const { return is_initialized_; }

	private:
		bool Init();

		bool Run();

		bool is_initialized_ = {};
		
		float acc_delta_time_ = kDefaultFixedDeltaTime;
		uint32 max_step_count_ = kDefaultMaxStepCount;
	};
}