#include "Engine/Core/pch.h"
#include "SpriteAnimClip.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimClip::SpriteAnimClip()
		:Super(SpriteAnimClip::kClassConcreteName)
	{}
	SpriteAnimClip::~SpriteAnimClip()
	{}
	void SpriteAnimClip::AddFrames(const std::vector<uint32>& frame_indices, float total_duration)
	{
		if (frame_indices.empty())
		{
			ERROR_MESSAGE("프레임이 비어 있습니다.");
			return;
		}
		if (total_duration <= 0.0f)
		{
			ERROR_MESSAGE("Total Duration은 양수여야 합니다.");
			return;
		}

		total_duration_ = total_duration;
		float frame_duration = total_duration / (float)frame_indices.size();

		for (uint32 index : frame_indices)
		{
			frames_.push_back({ index, frame_duration });
		}
	}
	void SpriteAnimClip::AddFrames(std::vector<Frame>&& frames)
	{
		for (const Frame& f : frames)
		{
			if (f.duration <= 0.0f)
			{
				ERROR_MESSAGE("Duration에 음수값이 있습니다. 확인하세요.");
				return;
			}
			total_duration_ += f.duration;

			max_frame_idx_ = std::max(max_frame_idx_, f.index);
		}
		frames_ = std::move(frames);
	}
}