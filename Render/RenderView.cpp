#include "Engine/Core/pch.h"
#include "RenderView.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Resource/GPU/RenderTargetGroup.h>

namespace engine
{
	void RenderView::SetCamera(const Camera& camera)
	{
		view_mat_ = camera.GetViewMatrix();
		proj_mat_ = camera.GetProjMatrix();
		view_proj_mat_ = view_mat_ * proj_mat_;	// 행 벡터 규약(v * M)

		position_ = camera.GetWorldPosition();
		forward_ = camera.GetWorldForward();

		is_orthographic_ = (camera.GetProjectionMatrixDesc().proj_mode == Camera::ProjectionMode::Orthographic);
		view_bounds_2d_ = is_orthographic_ ? camera.ComputeViewBounds2D() : AABB2D{};
	}

	void RenderView::SetOutput(RenderTargetGroup* target, const ClearSettings& clear_settings, const ViewportRect& viewport)
	{
		target_ = target;
		clear_settings_ = clear_settings;
		viewport_ = viewport;
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

		if (viewport_.IsFullTarget() == false)
		{
			const uint2 target_size = target_->GetTargetSize();

			D3D11_VIEWPORT vp = {};
			vp.TopLeftX = viewport_.x * static_cast<float>(target_size.x);
			vp.TopLeftY = viewport_.y * static_cast<float>(target_size.y);
			vp.Width = viewport_.width * static_cast<float>(target_size.x);
			vp.Height = viewport_.height * static_cast<float>(target_size.y);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			context->RSSetViewports(1, &vp);
		}
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