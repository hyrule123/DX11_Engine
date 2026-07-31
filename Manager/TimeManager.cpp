#include "Engine/Core/pch.h"
#include "TimeManager.h"

#include <Engine/Core/Constant.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	TimeManager::TimeManager()
	{
	}

	TimeManager::~TimeManager()
	{
	}

	void TimeManager::Init()
	{
		prev_time_point_ = Clock::now();
	}

	void TimeManager::Update()
	{
		TimePoint cur_timepoint = Clock::now();

		std::chrono::duration<float> duration = cur_timepoint - prev_time_point_;
		unscaled_delta_time_ = duration.count();
		scaled_delta_time_ = unscaled_delta_time_;

		//프레임이 늘어질 경우 강제로 capping
		if (kDeltaTimeCap < unscaled_delta_time_) { scaled_delta_time_ = kDeltaTimeCap; }

		delta_time_ = scaled_delta_time_;
		prev_time_point_ = cur_timepoint;
	}
}