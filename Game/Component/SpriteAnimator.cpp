#include "Engine/Core/pch.h"
#include "SpriteAnimator.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Resource/SpriteAnimation.h>
#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/SpriteAnimClip.h>
#include <Engine/Resource/Graphics/Buffer/Texture2DArray.h>

#include <Engine/Game/Component/SpriteRenderer.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimator::SpriteAnimator()
		: Super(SpriteAnimator::kClassConcreteName, ComponentCategory::kAnimator)
	{}
	SpriteAnimator::~SpriteAnimator()
	{}
	void SpriteAnimator::Init()
	{
		Super::Init();
	}
	void SpriteAnimator::Awake()
	{
		Super::Awake();
		s_ptr<SpriteRenderer> sprite_renderer = GetComponent<SpriteRenderer>();
		ASSERT_MESSAGE(sprite_renderer, "SpriteRenderer가 존재하지 않습니다.");
		renderer_ = sprite_renderer;
		
		ASSERT_MESSAGE(anim_, "Sprite Animation이 존재하지 않습니다.");
		ASSERT_MESSAGE(anim_->IsReady(), "Sprite Animation이 준비되지 않았습니다.");
	}
	void SpriteAnimator::LateUpdate()
	{
		Super::LateUpdate();

		//Animation Clip 있고, 재생 중이라면 로직 처리
		if (playing_clip_ && is_playing_)
		{
			// 시간 누적
			acc_deltatime_ += TimeManager::GetInst().DeltaTime();

			//while문을 사용하여
			while (acc_deltatime_ >= cur_frame_duration_)
			{
				// 시간 차감
				acc_deltatime_ -= cur_frame_duration_;

				// 다음 프레임으로 이동
				cur_frame_idx_++;

				// 애니메이션의 끝(마지막 프레임 초과)에 도달했을 때의 처리
				if (cur_frame_idx_ >= clip_frame_total_count_)
				{
					if (playing_clip_->IsLoop())
					{
						// 루프 진행
						cur_frame_idx_ = 0u;
					}
					else
					{
						// Non-loop: 마지막 프레임에 고정
						cur_frame_idx_ = (uint32)(playing_clip_->GetFrames().size() - 1);

						// 더 이상 시간이 누적되어 오버플로우가 발생하지 않도록 시간 고정
						acc_deltatime_ = playing_clip_->GetFrames()[cur_frame_idx_].duration;

						//스위치 OFF
						is_playing_ = false;

						break;
					}
				}

				// 다음 순회(Iteration)를 위해 다음 프레임의 duration으로 갱신
				cur_frame_duration_ = playing_clip_->GetFrames()[cur_frame_idx_].duration;
			}

			//참조해야 하는 TextureArray의 Index를 전달
			if (!renderer_.expired())
			{
				renderer_.lock()->SetFrameIndex(playing_clip_->GetFrames()[cur_frame_idx_].index);
			}
		}
	}
	bool SpriteAnimator::SetSpriteAnimation(const HashedStringView& res_path)
	{
		anim_ = ResourceManager::GetInst().Find<SpriteAnimation>(res_path);
		return (bool)anim_;
	}

	bool SpriteAnimator::Play(const HashedStringView& anim_name)
	{
		ASSERT(anim_);
		SpriteAnimClip* clip = anim_->GetAnimationClip(anim_name);
		if (clip)
		{
			PlayInternal(clip);
			return true;
		}

		return false;
	}
	bool SpriteAnimator::Play(SpriteAnimClip* clip_ptr)
	{
		ASSERT(anim_);
		if (clip_ptr && anim_->HasAnimationClip(clip_ptr))
		{
			PlayInternal(clip_ptr);
			return true;
		}

		return false;
	}
	bool SpriteAnimator::SwitchAnimKeepFrame(const HashedStringView& anim_name)
	{
		ASSERT(anim_);

		SpriteAnimClip* clip = anim_->GetAnimationClip(anim_name);
		if (clip)
		{
			return SwitchAnimKeepFrameInternal(clip);
		}
		return false;
	}
	bool SpriteAnimator::SwitchAnimKeepFrame(SpriteAnimClip* clip_ptr)
	{
		ASSERT(anim_);
		ASSERT(clip_ptr);

		if (clip_ptr && anim_->HasAnimationClip(clip_ptr))
		{
			return SwitchAnimKeepFrameInternal(clip_ptr);
		}
		return false;
	}
	void SpriteAnimator::PlayInternal(SpriteAnimClip* clip_ptr)
	{
		is_playing_ = true;
		acc_deltatime_ = 0.0f;
		cur_frame_idx_ = 0u;
		cur_frame_duration_ = clip_ptr->GetFrames()[0].duration;
		clip_frame_total_count_ = (uint32)clip_ptr->GetFrames().size();

		playing_clip_ = clip_ptr;
	}

	bool SpriteAnimator::SwitchAnimKeepFrameInternal(SpriteAnimClip* clip_ptr)
	{
		if (playing_clip_ && playing_clip_->GetFrames().size() == clip_ptr->GetFrames().size())
		{
			playing_clip_ = clip_ptr;

			//새 인덱스 계산을 위해 스위치 true로 변경
			is_playing_ = true;
			return true;
		}
		return false;
	}
}