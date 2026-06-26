#include "Engine/Core/pch.h"
#include "RenderTargetGroup.h"

#include <Engine/Resource/Graphics/Buffer/RenderTargetView.h>
#include <Engine/Resource/Graphics/Buffer/DepthStencilView.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	RenderTargetGroup::RenderTargetGroup()
		: Super(STRINGIFY(RenderTargetGroup))
	{
	}
	RenderTargetGroup::~RenderTargetGroup()
	{
	}
	void RenderTargetGroup::SetRenderTargets(const RenderTargetArray& RTVs)
	{
		render_target_buffers_ = RTVs;
		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				RTVs_ptr_cache_[i] = render_target_buffers_[i]->GetRawRTV();
			}
			else
			{
				RTVs_ptr_cache_[i] = nullptr;
			}
		}
	}
	bool RenderTargetGroup::CreateDepthStencilView(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc)
	{
		dsv_ = std::make_shared<DepthStencilView>();
		return dsv_->Create(device, desc);
	}
	void RenderTargetGroup::BindOutputMerger(ID3D11DeviceContext* context)
	{
		ID3D11DepthStencilView* dsv = nullptr;
		if (dsv_) { dsv = dsv_->GetRawDepthStencilView(); }
		context->OMSetRenderTargets((UINT)kMaxRenderTargetCount, RTVs_ptr_cache_.data(), dsv);
	}
	void RenderTargetGroup::ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color)
	{
		for (const auto& rtv : render_target_buffers_)
		{
			if (rtv)
			{
				context->ClearRenderTargetView(rtv->GetRawRTV(), clear_color.data());
			}
		}
	}
	void RenderTargetGroup::ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8 stencil)
	{
		if (dsv_ && dsv_->GetRawDepthStencilView())
		{
			context->ClearDepthStencilView(dsv_->GetRawDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, (FLOAT)depth, (UINT8)stencil);
		}
	}
}