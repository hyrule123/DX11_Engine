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
	void SpriteAnimation::AddAnimationClip(const std::string_view name, u_ptr<SpriteAnimClip> anim_clip)
	{
		if (!sprite_)
		{
			ERROR_MESSAGE("Sprite Texture를 먼저 설정하세요");
			return;
		}
		if (name.empty())
		{
			ERROR_MESSAGE("이름이 비어있음");
			return;
		}
		if (!anim_clip || !(anim_clip->IsReady()))
		{
			ERROR_MESSAGE("anim_clip이 nullptr");
			return;
		}
		if (sprite_->GetFrameCount() <= anim_clip->GetMaxFrameIndex())
		{
			ERROR_MESSAGE("최대 프레임 개수보다 높은 Frame Index가 있습니다.");
			return;
		}

		anim_clip_ptrs_.insert(anim_clip.get());
		anim_clips_[std::string(name)] = std::move(anim_clip);
	}

	SpriteAnimClip* SpriteAnimation::GetAnimationClip(const std::string_view anim_name) const
	{
		auto iter = anim_clips_.find(anim_name);

		if (iter != anim_clips_.end())
		{
			return iter->second.get();
		}
		return nullptr;
	}
}