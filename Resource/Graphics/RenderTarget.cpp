#include "Engine/Core/pch.h"
#include "RenderTarget.h"

namespace engine
{
	RenderTargetGroup::RenderTargetGroup()
		: Super(STRINGIFY(RenderTargetGroup))
	{
	}
	RenderTargetGroup::~RenderTargetGroup()
	{}
	void RenderTargetGroup::SetRenderTargets(RTVArray RTVs)
	{
		RTVs_ = RTVs;
		for (size_t i = 0; i < RTVs_.size(); ++i)
		{
			RTVs_ptr_cache_[i] = RTVs[i].Get();
		}
	}
	void RenderTargetGroup::BindOutputMerger(ID3D11DeviceContext* context)
	{
		context->OMSetRenderTargets((UINT)kMaxRenderTargetCount, RTVs_ptr_cache_.data(), DSV_.Get());
	}
	void RenderTargetGroup::ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color)
	{
		for (const auto& rtv : RTVs_)
		{
			if (rtv)
			{
				context->ClearRenderTargetView(rtv.Get(), clear_color.data());
			}
		}
		
	}
	void RenderTargetGroup::ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8 stencil)
	{
		if (DSV_)
		{
			context->ClearDepthStencilView(DSV_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, (FLOAT)depth, (UINT8)stencil);
		}
	}
}