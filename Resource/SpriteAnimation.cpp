#include "Engine/Core/pch.h"
#include "SpriteAnimation.h"

#include <Engine/Resource/SpriteAnimClip.h>
#include <Engine/Resource/Graphics/Buffer/Texture2DArray.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimation::SpriteAnimation()
		: Super(STRINGIFY(SpriteAnimation))
	{}
	SpriteAnimation::~SpriteAnimation()
	{}
	void SpriteAnimation::AddAnimationClip(const HashedStringView& name, u_ptr<SpriteAnimClip> anim_clip)
	{
		ASSERT(!!sprite_);
		ASSERT(name.GetStringView().size() > 0);
		ASSERT(anim_clip && anim_clip->IsReady());
		ASSERT(sprite_->GetFrameCount() > anim_clip->GetMaxFrameIndex());

		ASSERT(nullptr == anim_clips_.find(name));

		anim_clip_ptrs_.insert(anim_clip.get());
		anim_clips_.insert(name, std::move(anim_clip));
	}

	SpriteAnimClip* SpriteAnimation::GetAnimationClip(const HashedStringView& anim_name) const 
	{
		const u_ptr<SpriteAnimClip>* anim_clip_pp = anim_clips_.find(anim_name);
		if (anim_clip_pp)
		{
			return anim_clip_pp->get();
		}
		return nullptr;
	}
}