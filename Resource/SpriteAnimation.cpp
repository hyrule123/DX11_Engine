#include "Engine/Core/pch.h"
#include "SpriteAnimation.h"

#include <Engine/Resource/SpriteAnimClip.h>
#include <Engine/Resource/Graphics/Buffer/Texture2DArray.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimation::SpriteAnimation()
		: Super(SpriteAnimation::kClassConcreteName)
	{}
	SpriteAnimation::~SpriteAnimation()
	{}
	void SpriteAnimation::AddAnimationClip(const HashedStringView& name, u_ptr<SpriteAnimClip> anim_clip)
	{
		ASSERT(!!sprite_);
		ASSERT(name.GetStringView().size() > 0);
		ASSERT(anim_clip && anim_clip->IsReady());
		ASSERT(sprite_->GetFrameCount() > anim_clip->GetMaxFrameIndex());

		ASSERT(anim_clips_.find(name) == anim_clips_.end());

		anim_clip_ptrs_.insert(anim_clip.get());
		anim_clips_.insert(name, std::move(anim_clip));
	}

	SpriteAnimClip* SpriteAnimation::GetAnimationClip(const HashedStringView& anim_name) const 
	{
		auto iter = anim_clips_.find(anim_name);
		if (iter != anim_clips_.end())
		{
			return iter->second.get();
		}
		return nullptr;
	}
}