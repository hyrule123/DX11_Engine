#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/Constant.h>

#include <chrono>

namespace engine
{
	class TimeManager
	{
		DECLARE_SINGLETON(TimeManager)

	public:
		float GetDeltaTime() const { return delta_time_; }
		float GetFixedDeltaTime() const { return fixed_delta_time_; }

		constexpr float GetDeltaTimeCap() const { return kDeltaTimeCap; }

		void Init();
		void Update();

		void SetFixedUpdateMode(bool is_fixed_update) { delta_time_ = is_fixed_update ? fixed_delta_time_ : scaled_delta_time_; }

	private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		float unscaled_delta_time_ = {};
		float scaled_delta_time_ = {};

		float delta_time_ = {};

		TimePoint prev_time_point_ = {};

		float fixed_delta_time_ = kDefaultFixedDeltaTime;
	};
}


