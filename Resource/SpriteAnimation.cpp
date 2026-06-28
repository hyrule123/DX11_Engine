#include "Engine/Core/pch.h"
#include "SpriteAnimation.h"

namespace engine
{
	SpriteAnimation::SpriteAnimation()
		: Super(STRINGIFY(SpriteAnimation))
	{}
	SpriteAnimation::~SpriteAnimation()
	{}
	void SpriteAnimation::AddAnimationClip(const std::string_view name, const AnimationClip & anim_clip)
	{
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