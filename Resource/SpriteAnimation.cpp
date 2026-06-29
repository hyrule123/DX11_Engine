#include "Engine/Core/pch.h"
#include "SpriteAnimation.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimation::SpriteAnimation()
		: Super(STRINGIFY(SpriteAnimation))
	{}
	SpriteAnimation::~SpriteAnimation()
	{}
	void SpriteAnimation::AddAnimationClip(const std::string_view name, const AnimationClip & anim_clip)
	{
		//검증
		if(anim_clip.duration <= 0) 
		{
			ERROR_MESSAGE("Duration이 음수입니다.");
			return;
		}
		if (anim_clip.frames.empty())
		{
			ERROR_MESSAGE("Frame이 없습니다.");
			return;
		}

		anim_clips_[std::string(name)] = anim_clip;
	}
	const AnimationClip* SpriteAnimation::GetAnimationClip(const std::string_view anim_name) const
	{
		auto iter = anim_clips_.find(anim_name);

		if (iter != anim_clips_.end())
		{
			return &(iter->second);
		}
		return nullptr;
	}
}