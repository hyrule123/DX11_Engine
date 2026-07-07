#include "Engine/Core/pch.h"
#include "GraphicsShaderSet.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/Shader/InputLayoutDesc.h>
#include <Engine/Resource/Graphics/Shader/VertexShader.h>
#include <Engine/Resource/Graphics/State/RasterizerState.h>
#include <Engine/Resource/Graphics/Shader/PixelShader.h>
#include <Engine/Resource/Graphics/State/DepthStencilState.h>
#include <Engine/Resource/Graphics/State/BlendState.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	GraphicsShaderSet::GraphicsShaderSet()
		: Super(GraphicsShaderSet::kClassConcreteName)
	{
	}

	GraphicsShaderSet::~GraphicsShaderSet()
	{
	}

	bool GraphicsShaderSet::SetVertexShader(const stdfs::path& vs_name)
	{
		vertex_shader_ = ResourceManager::GetInst().LoadFromFile<VertexShader>(vs_name);
		return (bool)vertex_shader_;
	}
	void GraphicsShaderSet::SetVertexShader(s_ptr<VertexShader> vertex_shader)
	{
		if (!vertex_shader || !vertex_shader->IsReady())
		{
			ASSERT_MESSAGE(false, "VertexShader is not ready");
			return;
		}
		vertex_shader_ = std::move(vertex_shader);
	}
	bool GraphicsShaderSet::CreateInputLayout(const stdfs::path& layout_name)
	{
		s_ptr<InputLayoutDesc> desc = ResourceManager::GetInst().Find<InputLayoutDesc>(layout_name);
		return CreateInputLayout(desc);
	}

	bool GraphicsShaderSet::CreateInputLayout(s_ptr<InputLayoutDesc> desc)
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
		auto device = GraphicsDevice::GetInst().GetDevice();

		input_layout_ = desc->CreateInputLayout(device.Get(), vertex_shader_);

		return (bool)input_layout_;
	}

	bool GraphicsShaderSet::SetPixelShader(const stdfs::path& ps_name)
	{
		SetPixelShader(ResourceManager::GetInst().LoadFromFile<PixelShader>(ps_name));
		return (bool)pixel_shader_;
	}
	bool GraphicsShaderSet::SetRasterizerState(const stdfs::path& rss_name)
	{
		SetRasterizerState(ResourceManager::GetInst().Find<RasterizerState>(rss_name));
		return (bool)rasterizer_state_;
	}
	bool GraphicsShaderSet::SetBlendState(const stdfs::path& bs_name)
	{
		SetBlendState(ResourceManager::GetInst().Find<BlendState>(bs_name));
		return (bool)blend_state_;
	}
	bool GraphicsShaderSet::SetDepthStencilState(const stdfs::path& ds_name)
	{
		SetDepthStencilState(ResourceManager::GetInst().Find<DepthStencilState>(ds_name));
		return (bool)depth_stencil_state_;
	}
	void GraphicsShaderSet::Bind(ID3D11DeviceContext* context)
	{
		ASSERT(IsReady());

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
	}
	void GraphicsShaderSet::Clear(ID3D11DeviceContext* context)
	{
		context->IASetInputLayout(nullptr);
		context->VSSetShader(nullptr, nullptr, 0u);
		context->PSSetShader(nullptr, nullptr, 0u);
		context->RSSetState(nullptr);
		context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
		context->OMSetDepthStencilState(nullptr, 1u);
	}
}

