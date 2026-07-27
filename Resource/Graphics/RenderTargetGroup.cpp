#include "Engine/Core/pch.h"
#include "RenderTargetGroup.h"

#include <Engine/Resource/Graphics/Buffer/RenderTargetView.h>
#include <Engine/Resource/Graphics/Buffer/DepthStencilView.h>

#include <Engine/Core/Debug.h>

#include <array>

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

				RTVs_ptr_cache_[i] = render_target_buffers_[i]->GetRawRTV();
				SRVs_ptr_cache_[i] = render_target_buffers_[i]->GetRawSRV();
			}
			else
			{
				RTVs_ptr_cache_[i] = nullptr;
				SRVs_ptr_cache_[i] = nullptr;
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
					ASSERT_MESSAGE(false, "Size mismatch in RenderTargetGroup");
				}
			}
		}
#endif NDEBUG
	}

	void RenderTargetGroup::BindShaderResourceViews(ID3D11DeviceContext* context, ShaderStage::Flags stage_flags)
	{
		last_bound_stage_flags_ = stage_flags;

		UINT texcount = (UINT)SRVs_ptr_cache_.size();
		if (stage_flags & ShaderStage::kVS)
		{
			context->VSSetShaderResources(0u, texcount, SRVs_ptr_cache_.data());
		}
		if (stage_flags & ShaderStage::kGS)
		{
			context->GSSetShaderResources(0u, texcount, SRVs_ptr_cache_.data());
		}
		if (stage_flags & ShaderStage::kPS)
		{
			context->PSSetShaderResources(0u, texcount, SRVs_ptr_cache_.data());
		}
		if (stage_flags & ShaderStage::kCS)
		{
			context->CSSetShaderResources(0u, texcount, SRVs_ptr_cache_.data());
		}
	}

	void RenderTargetGroup::UnBindShaderResourceViews(ID3D11DeviceContext* context)
	{
		constexpr std::array<ID3D11ShaderResourceView*, kMaxTextureCount> null_srvs = {};

		UINT texcount = (UINT)SRVs_ptr_cache_.size();
		if (last_bound_stage_flags_ & ShaderStage::kVS)
		{
			context->VSSetShaderResources(0u, kMaxTextureCount, null_srvs.data());
		}
		if (last_bound_stage_flags_ & ShaderStage::kGS)
		{
			context->GSSetShaderResources(0u, kMaxTextureCount, null_srvs.data());
		}
		if (last_bound_stage_flags_ & ShaderStage::kPS)
		{
			context->PSSetShaderResources(0u, kMaxTextureCount, null_srvs.data());
		}
		if (last_bound_stage_flags_ & ShaderStage::kCS)
		{
			context->CSSetShaderResources(0u, kMaxTextureCount, null_srvs.data());
		}

		last_bound_stage_flags_ = ShaderStage::kNone;
	}

	void RenderTargetGroup::BindOutputMerger(ID3D11DeviceContext* context)
	{
		ID3D11DepthStencilView* dsv = nullptr;
		if (dsv_) { dsv = dsv_->GetRawDepthStencilView(); }
		context->OMSetRenderTargets((UINT)kMaxRenderTargetCount, RTVs_ptr_cache_.data(), dsv);
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
		RTVs_ptr_cache_.fill(nullptr);
	}
	void RenderTargetGroup::Resize( uint32 width, uint32 height)
	{
		if (!requires_resize_) 
		{ 
			std::string msg = "No resize needed for RenderTargetGroup";
			DEBUG_LOG_A(msg.c_str());
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