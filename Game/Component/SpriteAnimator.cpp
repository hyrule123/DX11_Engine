#include "Engine/Core/pch.h"
#include "SpriteAnimator.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Resource/SpriteAnimation.h>
#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/Graphics/Buffer/Texture2DArray.h>

#include <Engine/Game/Component/SpriteRenderer.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteAnimator::SpriteAnimator()
		: Super(STRINGIFY(SpriteAnimator), ComponentCategory::kAnimator)
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
		renderer_ = GetComponent<SpriteRenderer>();
		if (renderer_.expired())
		{
			DEBUG_LOG("Sprite Renderer 준비되지 않아 렌더링 불가");
			return;
		}
		
		if (!anim_ || !(anim_->IsReady()))
		{
			DEBUG_LOG("Sprite Animation 준비되지 않아 렌더링 불가");
			return;
		}
		
		//해당 Animation만의 Material Key 생성.
		stdfs::path uniq_mtrl_key = anim_->GetPath();
		uniq_mtrl_key += L"_Material";
		
		//고유 Material을 찾는다
		auto& res_mgr = ResourceManager::GetInst();
		s_ptr<Material> mtrl = res_mgr.Find<Material>(uniq_mtrl_key);

		//고유 Material이 없을 경우 새로 생성 후 Renderer에 텍스처 지정
		if (!mtrl)
		{
			mtrl = res_mgr.Find<Material>("Sprite_Material")->Clone();
			mtrl->SetTextures({ anim_->GetSprite(), });
			renderer_.lock()->SetMaterial(mtrl);
		}
	}
	void SpriteAnimator::LateUpdate()
	{
		Super::LateUpdate();

		if (playing_clip_ && is_playing_)
		{
			const AnimationClip& clip = *playing_clip_;

			acc_deltatime_ += TimeManager::GetInst().DeltaTime();

			//break용
			do
			{
				// 루프(반복)하지 않는 애니메이션의 종료 처리 로직
				if (!clip.is_loop && acc_deltatime_ >= clip.duration)
				{
					// 시간을 최대치로 고정 (Clamp)
					acc_deltatime_ = clip.duration;
					// 마지막 프레임 인덱스로 고정
					cur_frame_idx_ = (uint32)(clip.frames.size() - 1);

					is_playing_ = false;
					
					break;
				}

				// 루프(반복)하는 애니메이션의 초과 시간 정리 로직 (렉 스파이크 대비)
				if (clip.is_loop)
				{
					// while문을 사용하여 초과한 시간만큼 모두 빼줌 (fmodf를 사용해도 무방함)
					while (acc_deltatime_ >= clip.duration)
					{
						acc_deltatime_ -= clip.duration;
					}
				}

				// 계산된 남은 시간을 바탕으로 현재 프레임 인덱스 도출
				cur_frame_idx_ = (uint32)(acc_deltatime_ / time_per_frame_);

				uint32 frames_count = (uint32)clip.frames.size();
				// 부동소수점 오차로 인해 발생할 수 있는 Out of Range 방어
				if (cur_frame_idx_ >= frames_count)
				{
					cur_frame_idx_ = frames_count - 1;
				}
			} while (false);


			if (!renderer_.expired())
			{
				renderer_.lock()->SetTestFrame(clip.frames[cur_frame_idx_]);
			}
		}
	}
	bool SpriteAnimator::SetSpriteAnimation(const stdfs::path& res_path)
	{
		anim_ = ResourceManager::GetInst().Find<SpriteAnimation>(res_path);
		return (bool)anim_;
	}
	bool SpriteAnimator::Play(const std::string_view anim_name)
	{
		if (!anim_) { return false; }
		playing_clip_ = anim_->GetAnimationClip(anim_name);
		if (playing_clip_)
		{
			is_playing_ = true;
			acc_deltatime_ = 0.0f;
			cur_frame_idx_ = 0u;
			time_per_frame_ = playing_clip_->duration / (float)(playing_clip_->frames.size());
			return true;
		}

		return false;
	}
}