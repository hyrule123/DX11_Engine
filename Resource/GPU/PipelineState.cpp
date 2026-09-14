#include "Engine/Core/pch.h"
#include "PipelineState.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/Shader/InputLayoutDesc.h>
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
		vertex_shader_ = ResourceManager::GetInst().LoadFromFile<VertexShader>(vs_name);
		return (bool)vertex_shader_;
	}
	void PipelineState::SetVertexShader(s_ptr<VertexShader> vertex_shader)
	{
		if (!vertex_shader || !vertex_shader->IsReady())
		{
			ASSERT_F(false, "VertexShader is not ready");
			return;
		}
		vertex_shader_ = std::move(vertex_shader);
	}
	bool PipelineState::CreateInputLayout(const HashedStringView& layout_name)
	{
		s_ptr<InputLayoutDesc> desc = ResourceManager::GetInst().Find<InputLayoutDesc>(layout_name);
		return CreateInputLayout(desc.get());
	}

	bool PipelineState::CreateInputLayout(InputLayoutDesc* desc)
	{
		if (!vertex_shader_)
		{
			ASSERT_F(false, "VertexShader is not ready");
			return false;
		}

		if (!desc || !desc->IsReady())
		{
			ASSERT_F(false, "InputLayoutDesc is not ready");
			return false;
		}

		const auto& descs = desc->GetInputLayoutDesc();
		const auto& byte_code = vertex_shader_->GetByteCode();

		input_layout_ = desc->CreateInputLayout(vertex_shader_.get());

		return (bool)input_layout_;
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
	void PipelineState::Bind(ID3D11DeviceContext* context)
	{
		if (!IsReady()) 
		{ 
			ASSERT_F(false, "PipelineState is not ready. Ensure that the input layout and vertex shader are set before binding.");
			return; 
		}

		context->IASetInputLayout(input_layout_.Get());

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
	void PipelineState::Clear(ID3D11DeviceContext* context)
	{
		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, nullptr, 0u);
		context->PSSetShader(nullptr, nullptr, 0u);
		context->RSSetState(nullptr);
		context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
		context->OMSetDepthStencilState(nullptr, 1u);
	}
}

