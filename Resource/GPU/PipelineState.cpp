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

namespace engine
{
	PipelineState::PipelineState()
		: Super(PipelineState::kClassConcreteName)
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
			ASSERT_MESSAGE(false, "VertexShader is not ready");
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
			ASSERT_MESSAGE(false, "VertexShader is not ready");
			return false;
		}

		if (!desc || !desc->IsReady())
		{
			ASSERT_MESSAGE(false, "InputLayoutDesc is not ready");
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
			DEBUG_MESSAGE("PipelineState is not ready. Ensure that the input layout and vertex shader are set before binding.");
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

		for (const auto& binding : constant_buffer_bindings_)
		{
			ASSERT(binding.constant_buffer != nullptr);

			binding.constant_buffer->Bind(context, binding.stage_flag, binding.slot);
		}

		for (const auto& binding : shader_resource_bindings_)
		{
			ASSERT(binding.shader_resource != nullptr);

			ID3D11ShaderResourceView* srv = binding.shader_resource->GetSRV();
			if (srv == nullptr)
			{
				DEBUG_LOG("ShaderResourceView is nullptr for slot. Skipping binding.");
				continue;
			}

			const auto stage = binding.stage_flag;
			if (stage & ShaderStage::kVS) { context->VSSetShaderResources(binding.slot, 1, &srv); }
			if (stage & ShaderStage::kGS) { context->GSSetShaderResources(binding.slot, 1, &srv); }
			if (stage & ShaderStage::kPS) { context->PSSetShaderResources(binding.slot, 1, &srv); }

			// CS는 Graphics Pipeline State에서 직접적으로 바인딩하지 않음.
			//if (stage & ShaderStage::kCS) { context->CSSetShaderResources(binding.slot, 1, &srv); }
		}
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

	void PipelineState::AddConstantBufferBinding(uint32 slot, ShaderStage::Flags stage_flag, s_ptr<ConstantBuffer> constant_buffer)
	{
		if (constant_buffer == nullptr)
		{
			DEBUG_MESSAGE("ConstantBuffer is nullptr.");
			return;
		}
		if (stage_flag == ShaderStage::kNone)
		{
			DEBUG_MESSAGE("Invalid stage_flag: ShaderStage::kNone");
			return;
		}
		if (slot >= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
		{
			DEBUG_MESSAGE("Invalid slot");
			return;
		}

		for (size_t i = 0; i < constant_buffer_bindings_.size(); ++i)
		{
			const auto& binding = constant_buffer_bindings_[i];

			// 바인딩은 슬롯당 하나
			if (binding.slot == slot)
			{
				ASSERT_MESSAGE(false, "ConstantBufferBinding already exists for the given slot");
				return;
			}
		}
		constant_buffer_bindings_.push_back({ stage_flag, slot, std::move(constant_buffer) });
	}

	void PipelineState::RemoveConstantBufferBinding(uint32 slot)
	{
		for (size_t i = 0; i < constant_buffer_bindings_.size(); ++i)
		{
			if (constant_buffer_bindings_[i].slot == slot)
			{
				std::swap(constant_buffer_bindings_[i], constant_buffer_bindings_.back());
				constant_buffer_bindings_.pop_back();
				return;
			}
		}
		ERROR_MESSAGE("No ConstantBufferBinding found for the given slot to remove.");
	}

	void PipelineState::AddShaderResourceBinding(uint32 slot, ShaderStage::Flags stage_flag, s_ptr<ShaderResource> shader_resource)
	{
		if (shader_resource == nullptr)
		{
			ASSERT_MESSAGE(false, "ShaderResource is nullptr");
			return;
		}

		for (size_t i = 0; i < shader_resource_bindings_.size(); ++i)
		{
			// 바인딩은 슬롯당 하나
			const auto& binding = shader_resource_bindings_[i];
			if (binding.slot == slot)
			{
				ASSERT_MESSAGE(false, "ShaderResourceBinding already exists for the given slot");
				return;
			}
		}

		shader_resource_bindings_.push_back({ stage_flag, slot, std::move(shader_resource) });
	}
	void PipelineState::RemoveShaderResourceBinding(uint32 slot)
	{
		for (size_t i = 0; i < shader_resource_bindings_.size(); ++i)
		{
			if (shader_resource_bindings_[i].slot == slot)
			{
				std::swap(shader_resource_bindings_[i], shader_resource_bindings_.back());
				shader_resource_bindings_.pop_back();
				return;
			}
		}
		ERROR_MESSAGE("No ShaderResourceBinding found for the given slot to remove.");
	}
}

