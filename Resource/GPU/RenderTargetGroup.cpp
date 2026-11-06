#include "Engine/Core/pch.h"
#include "RenderTargetGroup.h"

#include <Engine/Resource/GPU/Buffer/RenderTarget.h>
#include <Engine/Resource/GPU/Buffer/DepthStencilTarget.h>

#include <Engine/Core/Debug.h>

#include <array>
#include <utility>

namespace engine
{
	RenderTargetGroup::RenderTargetGroup()
		: Super(RenderTargetGroup::kClassConcreteName)
	{
	}
	RenderTargetGroup::~RenderTargetGroup()
	{
	}

	void RenderTargetGroup::SetRenderTargets(const RenderTargetArray& rtv_arr)
	{
		render_target_buffers_ = rtv_arr;
		
		//VERIFY
#ifndef NDEBUG
		const uint32_2 target_size = GetTargetSize();
		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				if (target_size.x != render_target_buffers_[i]->GetWidth() ||
					target_size.y != render_target_buffers_[i]->GetHeight())
				{
					ASSERT_F(false, "Size mismatch in RenderTargetGroup");
				}
			}
		}
#endif NDEBUG
	}

	void RenderTargetGroup::BindSRVs(ID3D11DeviceContext* context, ShaderStageFlags stage_flags, RegisterT start_slot)
	{
		srv_bound_ = true;

		constexpr size_t tex_count = std::tuple_size_v<RenderTargetArray>;
		std::array<ID3D11ShaderResourceView*, tex_count> srvs = {};

		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				srvs[i] = render_target_buffers_[i]->GetRawSRV();
			}
		}
		
		if (stage_flags.Test(ShaderStage::Vertex))
		{
			context->VSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Geometry))
		{
			context->GSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Pixel))
		{
			context->PSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Compute))
		{
			context->CSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
	}

	void RenderTargetGroup::UnbindSRVs(ID3D11DeviceContext* context, ShaderStageFlags stage_flags, RegisterT start_slot)
	{
		constexpr size_t tex_count = std::tuple_size_v<RenderTargetArray>;

		std::array<ID3D11ShaderResourceView*, tex_count> null_srvs = {};

		if (stage_flags.Test(ShaderStage::Vertex))
		{
			context->VSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Geometry))
		{
			context->GSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Pixel))
		{
			context->PSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (stage_flags.Test(ShaderStage::Compute))
		{
			context->CSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}

		srv_bound_ = false;
	}

	void RenderTargetGroup::BindOutputMerger(ID3D11DeviceContext* context)
	{
		// SRV 바인딩을 해제하지 않았을 경우 에러
		/* 자동 언바인드 하지 않는 이유 
		중간에 다른 SRV에서 바인딩이 일어날 수 있기 때문에, 에러를 발생시키고 처리 코드를 추가하도록 하는 게 더 안전함. SRV 바인딩 후 OM 바인딩 전에 반드시 UnbindSRV를 호출하도록 강제.
		*/
		CHECK(srv_bound_ == false);

		ID3D11DepthStencilView* dsv = nullptr;
		if (dsv_) { dsv = dsv_->GetRawDepthStencilView(); }

		constexpr size_t tex_count = std::tuple_size_v<RenderTargetArray>;
		std::array<ID3D11RenderTargetView*, tex_count> rtvs = {};

		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				rtvs[i] = render_target_buffers_[i]->GetRawRTV();
			}
		}

		context->OMSetRenderTargets((UINT)tex_count, rtvs.data(), dsv);

		D3D11_VIEWPORT viewport = {};
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		uint32_2 target_size = GetTargetSize();

		viewport.Width = static_cast<FLOAT>(target_size.x);
		viewport.Height = static_cast<FLOAT>(target_size.y);

		context->RSSetViewports(1, &viewport);
	}
	void RenderTargetGroup::UnbindOutputMerger(ID3D11DeviceContext* context)
	{
		context->OMSetRenderTargets(0, nullptr, nullptr);
	}
	void RenderTargetGroup::Clear(ID3D11DeviceContext* context, const ClearSettings& settings)
	{
		if (settings.flags.Test(ClearTarget::Color))
		{
			ClearRenderTargetView(context, settings.color);
		}
		if (settings.flags.Test(ClearTarget::DepthStencil))
		{
			ClearDepthStencilView(context, settings.depth, settings.stencil);
		}
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
	void RenderTargetGroup::ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8_t stencil)
	{
		if (dsv_ && dsv_->GetRawDepthStencilView())
		{
			context->ClearDepthStencilView(dsv_->GetRawDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, (FLOAT)depth, (UINT8)stencil);
		}
	}
	void RenderTargetGroup::Reset()
	{
		render_target_buffers_.fill(nullptr);
		dsv_ = nullptr;
	}

	uint2 RenderTargetGroup::GetTargetSize() const
	{
		for (const auto& render_target : render_target_buffers_)
		{
			if (render_target)
			{
				return { render_target->GetWidth(), render_target->GetHeight() };
			}
		}

		if (dsv_)
		{
			return { dsv_->GetWidth(), dsv_->GetHeight() };
		}
		return {};
	}
}