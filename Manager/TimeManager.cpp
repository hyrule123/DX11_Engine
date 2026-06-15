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

	void TimeManager::Update()
	{
		TimePoint cur_timepoint = Clock::now();

		std::chrono::duration<float> duration = cur_timepoint - prev_time_point_;
		float raw_deltatime = duration.count();

		//프레임이 늘어질 경우 강제로 capping
		if (kDeltaTimeCap < raw_deltatime) { raw_deltatime = kDeltaTimeCap; }

		delta_time_ = raw_deltatime;
		prev_time_point_ = cur_timepoint;

		//std::string timestr = std::to_string(delta_time_);
		//DEBUG_LOG_A(timestr.c_str());
	}
}