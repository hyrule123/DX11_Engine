#include "Engine/Core/pch.h"
#include "RenderTargetGroup.h"

#include <Engine/Resource/GPU/Buffer/RenderTargetView.h>
#include <Engine/Resource/GPU/Buffer/DepthStencilView.h>

#include <Engine/Core/Debug.h>

#include <array>
#include <utility>

namespace engine
{
	RenderTargetGroup::RenderTargetGroup()
		: Super(RenderTargetGroup::kClassConcreteName)
	{
#pragma region Viewport
		//Viewport 생성
		viewport_.TopLeftX = 0.0f;
		viewport_.TopLeftY = 0.0f;

		//Width와 Height는 렌더타겟의 크기에 맞게 설정되어야 함
		viewport_.Width = 0.0f;
		viewport_.Height = 0.0f;

		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;
#pragma endregion Viewport
	}
	RenderTargetGroup::~RenderTargetGroup()
	{
	}
	void RenderTargetGroup::SetRenderTargets(const RenderTargetArray& RTVs)
	{
		render_target_buffers_ = RTVs;

		bool size_found = false;

		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				if (!size_found)
				{
					viewport_.Width = (float)render_target_buffers_[i]->GetWidth();
					viewport_.Height = (float)render_target_buffers_[i]->GetHeight();
					size_found = true;
				}
			}
		}

//VERIFY
#ifndef NDEBUG
		for (size_t i = 0; i < render_target_buffers_.size(); ++i)
		{
			if (render_target_buffers_[i])
			{
				if (viewport_.Width != (float)render_target_buffers_[i]->GetWidth() ||
					viewport_.Height != (float)render_target_buffers_[i]->GetHeight())
				{
					ASSERT_F(false, "Size mismatch in RenderTargetGroup");
				}
			}
		}
#endif NDEBUG
	}

	void RenderTargetGroup::BindShaderResourceViews(ID3D11DeviceContext* context, ShaderStage::Flags stage_flags, RegisterT start_slot)
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
		
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Vertex))
		{
			context->VSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Geometry))
		{
			context->GSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Pixel))
		{
			context->PSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Compute))
		{
			context->CSSetShaderResources(start_slot.Get(), (UINT)tex_count, srvs.data());
		}
	}

	void RenderTargetGroup::UnBindShaderResourceViews(ID3D11DeviceContext* context, ShaderStage::Flags stage_flags, RegisterT start_slot)
	{
		constexpr size_t tex_count = std::tuple_size_v<RenderTargetArray>;

		std::array<ID3D11ShaderResourceView*, tex_count> null_srvs = {};

		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Vertex))
		{
			context->VSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Geometry))
		{
			context->GSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Pixel))
		{
			context->PSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}
		if (ShaderStage::HasFlag(stage_flags, ShaderStage::Flags::Compute))
		{
			context->CSSetShaderResources(start_slot.Get(), (UINT)tex_count, null_srvs.data());
		}

		srv_bound_ = false;
	}

	void RenderTargetGroup::BindOutputMerger(ID3D11DeviceContext* context)
	{
		// SRV 바인딩을 해제하지 않았을 경우 에러
		/* 자동 언바인드 하지 않는 이유 
		중간에 다른 SRV에서 바인딩이 일어날 수 있기 때문에, 에러를 발생시키고 처리 코드를 추가하도록 하는 게 더 안전함. SRV 바인딩 후 OM 바인딩 전에 반드시 UnBindShaderResourceViews를 호출하도록 강제.
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
		context->RSSetViewports(1, &viewport_);
	}
	void RenderTargetGroup::UnBindOutputMerger(ID3D11DeviceContext* context)
	{
		context->OMSetRenderTargets(0, nullptr, nullptr);
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
	void RenderTargetGroup::Reset()
	{
		render_target_buffers_.fill(nullptr);
		dsv_ = nullptr;
	}
	void RenderTargetGroup::Resize(uint32 width, uint32 height)
	{
		if (!requires_resize_) 
		{ 
			DEBUG_LOG("No resize needed for RenderTargetGroup");
			return;
		}

		for (auto& rtv : render_target_buffers_)
		{
			if (rtv)
			{
				rtv->Resize(width, height);
			}
		}
		if (dsv_)
		{
			dsv_->Resize(width, height);
		}
	}
}