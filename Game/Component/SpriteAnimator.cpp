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

		if (playing_clip_)
		{
			const AnimationClip& clip = *playing_clip_;

			acc_deltatime_ += TimeManager::GetInst().DeltaTime();

			if (acc_deltatime_ > clip.duration)
			{
				if (clip.is_loop)
				{
					cur_frame_idx_ = 0u;
					acc_deltatime_ -= clip.duration;
				}
			}
			else
			{
				uint32 frames_count = (uint32)clip.frames.size();
				float time_per_frame = clip.duration / (float)frames_count;

				cur_frame_idx_ = (uint32)(acc_deltatime_ / time_per_frame);
			}

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
			acc_deltatime_ = 0.0f;
			cur_frame_idx_ = 0u;
			return true;
		}

		return false;
	}
}