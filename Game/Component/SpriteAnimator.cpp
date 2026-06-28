#include "Engine/Core/pch.h"
#include "SpriteAnimator.h"

#include <Engine/Manager/ResourceManager.h>

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


	}
	bool SpriteAnimator::SetSpriteAnimation(const stdfs::path& res_path)
	{
		anim_ = ResourceManager::GetInst().Find<SpriteAnimation>(res_path);
		return (bool)anim_;
	}
}