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
		inline float DeltaTime() const { return delta_time_; }

		void Update();

	private:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		float delta_time_ = {};

		TimePoint prev_time_point_ = {};
	};
}


