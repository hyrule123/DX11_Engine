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
			acc_deltatime_ += TimeManager::GetInst().GetDeltaTime();

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

						//노티파이 확인 배열 초기화
						std::fill(frame_notify_checked_.begin(), frame_notify_checked_.end(), false);
					}
					else
					{
						// Non-loop: 마지막 프레임에 고정
						cur_frame_idx_ = (uint32)(playing_clip_->GetFrames().size() - 1);

						// 더 이상 시간이 누적되어 오버플로우가 발생하지 않도록 시간 고정
						acc_deltatime_ = playing_clip_->GetFrames()[cur_frame_idx_].duration;

						//스위치 OFF
						is_playing_ = false;
					}
				}

				// 다음 순회(Iteration)를 위해 다음 프레임의 duration으로 갱신
				cur_frame_duration_ = playing_clip_->GetFrames()[cur_frame_idx_].duration;
				
				if (!is_playing_) { break; }

				//재생이 끝난 게 아니면 해당 프레임의 Notify 처리
				TriggerNotify(cur_frame_idx_);
			}

			//참조해야 하는 TextureArray의 Index를 전달
			if (auto renderer = renderer_.lock())
			{
				renderer->SetFrameIndex(playing_clip_->GetFrames()[cur_frame_idx_].index);
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
		frame_notify_checked_.clear();
		frame_notify_checked_.resize(clip_ptr->GetFrames().size(), false);
		clip_frame_total_count_ = (uint32)clip_ptr->GetFrames().size();
		playing_clip_ = clip_ptr;

		// Check 0 frame notify - 업데이트 루프에서 첫 0프레임은 노티파이 처리 안되므로 여기서 처리
		TriggerNotify(0);
	}

	bool SpriteAnimator::SwitchAnimKeepFrameInternal(SpriteAnimClip* clip_ptr)
	{
		//clip_ptr의 null check는 이미 호출한 함수에서 처리했으므로 생략
		if (playing_clip_ && playing_clip_->GetFrames().size() == clip_ptr->GetFrames().size())
		{
			playing_clip_ = clip_ptr;

			//참조해야 하는 TextureArray의 Index를 새로 고침
			if (auto renderer = renderer_.lock())
			{
				renderer->SetFrameIndex(playing_clip_->GetFrames()[cur_frame_idx_].index);
			}

			return true;
		}
		return false;
	}

	//No Exception Check(다 확인했다고 가정)
	void SpriteAnimator::TriggerNotify(uint32 frame_idx)
	{
		if (frame_notify_checked_[frame_idx]) { return; }
		ASSERT(frame_idx < (uint32)frame_notify_checked_.size());
		frame_notify_checked_[frame_idx] = true;
		// 노티파이 있을 시 호출
		HashedStringView notify_name = playing_clip_->GetFrameNotifyName(frame_idx);
		if (false == notify_name.IsEmpty())
		{
			auto iter = frame_notify_callbacks_.find(notify_name);
			if (iter != frame_notify_callbacks_.end())
			{
				for (const auto& callback : iter->second)
				{
					if (callback)
					{
						callback();
					}
				}
			}
		}
	}
}