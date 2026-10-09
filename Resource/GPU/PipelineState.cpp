#include "Engine/Core/pch.h"
#include "PipelineState.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/Shader/InputLayout.h>
#include <Engine/Resource/GPU/Shader/VertexShader.h>
#include <Engine/Resource/GPU/State/RasterizerState.h>
#include <Engine/Resource/GPU/Shader/PixelShader.h>
#include <Engine/Resource/GPU/State/DepthStencilState.h>
#include <Engine/Resource/GPU/State/BlendState.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>
#include <Engine/Resource/GPU/Buffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/Core/Register.hlsli>

namespace engine
{
	PipelineState::PipelineState()
		: Super(PipelineState::kClassConcreteName)
		, per_pipeline_buffer_binding_table_(REG_B_PER_PIPELINE_START, REG_B_PER_PIPELINE_COUNT, REG_T_PER_PIPELINE_START, REG_T_PER_PIPELINE_COUNT)
	{
	}

	PipelineState::~PipelineState()
	{
	}

	bool PipelineState::SetVertexShader(const HashedStringView& vs_name)
	{
		std::shared_ptr<VertexShader> vs = ResourceManager::GetInst().LoadFromFile<VertexShader>(vs_name);
		bool is_vs = (bool)vs;
		if (is_vs) { SetVertexShader(std::move(vs)); }
		return is_vs;
	}
	void PipelineState::SetVertexShader(std::shared_ptr<VertexShader> vertex_shader)
	{
		if (!vertex_shader || !vertex_shader->IsReady())
		{
			ASSERT_F(false, "VertexShader is not ready");
			return;
		}
		vertex_shader_ = std::move(vertex_shader);
	}

	bool PipelineState::SetInputLayout(const HashedStringView& name)
	{
		std::shared_ptr<InputLayout> il = ResourceManager::GetInst().Find<InputLayout>(name);
		bool is_il = (bool)il;
		SetInputLayout(std::move(il));
		return is_il;
	}

	bool PipelineState::CreateInputLayout(std::span<const D3D11_INPUT_ELEMENT_DESC> layout_descs)
	{
		if (layout_descs.empty())
		{
			ASSERT_F(false, "InputLayout is empty");
			return false;
		}
		// VS 체크는 InputLayout::Create에서 진행

		input_layout_ = EntityManager::CreateEntity<InputLayout>();
		return input_layout_->Create(layout_descs, vertex_shader_.get());
	}

	bool PipelineState::SetPixelShader(const HashedStringView& ps_name)
	{
		SetPixelShader(ResourceManager::GetInst().LoadFromFile<PixelShader>(ps_name));
		return (bool)pixel_shader_;
	}
	bool PipelineState::SetRasterizerState(const HashedStringView& rss_name)
	{
		SetRasterizerState(ResourceManager::GetInst().Find<RasterizerState>(rss_name));
		return (bool)rasterizer_state_;
	}
	bool PipelineState::SetBlendState(const HashedStringView& bs_name)
	{
		SetBlendState(ResourceManager::GetInst().Find<BlendState>(bs_name));
		return (bool)blend_state_;
	}
	bool PipelineState::SetDepthStencilState(const HashedStringView& ds_name)
	{
		SetDepthStencilState(ResourceManager::GetInst().Find<DepthStencilState>(ds_name));
		return (bool)depth_stencil_state_;
	}
	void PipelineState::Bind(ID3D11DeviceContext* context) const
	{
		if (!IsReady()) 
		{ 
			ASSERT_F(false, "PipelineState is not ready. Ensure that the input layout and vertex shader are set before binding.");
			return; 
		}

		if (input_layout_) { input_layout_->Bind(context); }
		else { context->IASetInputLayout(nullptr); }

		if (vertex_shader_) { vertex_shader_->Bind(context); }
		else { context->VSSetShader(nullptr, nullptr, 0u); }

		if (pixel_shader_) { pixel_shader_->Bind(context); }
		else { context->PSSetShader(nullptr, nullptr, 0u); }
		
		if (rasterizer_state_) { rasterizer_state_->Bind(context); }
		else { context->RSSetState(nullptr); }

		if (blend_state_) { blend_state_->Bind(context); }
		else { context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); }

		if (depth_stencil_state_) { depth_stencil_state_->Bind(context); }
		else { context->OMSetDepthStencilState(nullptr, 1u); }
		
		per_pipeline_buffer_binding_table_.Bind(context);
	}
	void PipelineState::ClearAllBindings(ID3D11DeviceContext* context)
	{
		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, nullptr, 0u);
		context->PSSetShader(nullptr, nullptr, 0u);
		context->RSSetState(nullptr);
		context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
		context->OMSetDepthStencilState(nullptr, 1u);
	}
}

