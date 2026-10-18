#pragma once

#include <Engine/Core/Singleton.h>
#include <Engine/Core/CoreTypes.h>
#include <Engine/Core/Constants.h>

namespace engine
{
	class GameEngine
	{
		DECLARE_SINGLETON(GameEngine)

		friend class EngineMain;

	public:
		void OnScreenSizeChange(uint32_t width, uint32_t height);
		bool IsInitialized() const { return is_initialized_; }

		uint64_t GetFrameCount() const { return frame_count_; }

	private:
		bool Init();

		bool Run();

		bool is_initialized_ = {};
		
		float acc_delta_time_ = kDefaultFixedDeltaTime;
		uint32_t max_step_count_ = kDefaultMaxStepCount;

		uint64_t frame_count_ = {};
	};
}