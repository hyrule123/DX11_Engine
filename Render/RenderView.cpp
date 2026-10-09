#include "Engine/Core/pch.h"
#include "RenderView.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Resource/GPU/RenderTargetGroup.h>

namespace engine
{
	void RenderView::Setup(const Camera& camera, RenderTargetGroup* target, const ClearSettings& clear_settings)
	{
		view_mat_ = camera.GetViewMatrix();
		proj_mat_ = camera.GetProjMatrix();
		view_proj_mat_ = view_mat_ * proj_mat_;	// 행 벡터 규약(v * M)

		position_ = camera.GetWorldPosition();
		forward_ = camera.GetWorldForward();

		is_orthographic_ = (camera.GetProjectionMatrixDesc().proj_mode == Camera::ProjectionMode::Orthographic);
		view_bounds_2d_ = is_orthographic_ ? camera.ComputeViewBounds2D() : AABB2D{};

		target_ = target;
		clear_settings_ = clear_settings;
	}

	void RenderView::BindTargets(ID3D11DeviceContext* context) const
	{
		ASSERT_F(target_ != nullptr, "RenderView에 출력 대상이 설정되지 않았습니다.");
		if (target_ == nullptr)
		{
			return;
		}

		// RTV/DSV + 뷰포트 함께 설정
		target_->BindOutputMerger(context);
	}

	void RenderView::ClearTargets(ID3D11DeviceContext* context) const
	{
		if (target_ == nullptr)
		{
			return;
		}
		
		target_->Clear(context, clear_settings_);
	}
}