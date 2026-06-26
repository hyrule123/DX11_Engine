#include "Engine/Core/pch.h"
#include "GraphicsPipeline.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/InputLayout.h>
#include <Engine/Resource/Graphics/VertexShader.h>
#include <Engine/Resource/Graphics/PixelShader.h>
#include <Engine/Resource/Graphics/RasterizerState.h>
#include <Engine/Resource/Graphics/BlendState.h>
#include <Engine/Resource/Graphics/DepthStencilState.h>

namespace engine
{
	GraphicsPipeline::GraphicsPipeline()
		: Resource(STRINGIFY(GraphicsPipeline))
	{
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
	}
	bool GraphicsPipeline::SetInputLayout(const stdfs::path& layout_name)
	{
		input_layout_ = ResourceManager::GetInst().Find<InputLayout>(layout_name);
		return (bool)input_layout_;
	}
	bool GraphicsPipeline::SetVertexShader(const stdfs::path& vs_name)
	{
		vertex_shader_ = ResourceManager::GetInst().LoadFromFile<VertexShader>(vs_name);
		return (bool)vertex_shader_;
	}
	bool GraphicsPipeline::SetPixelShader(const stdfs::path& ps_name)
	{
		pixel_shader_ = ResourceManager::GetInst().LoadFromFile<PixelShader>(ps_name);
		return (bool)pixel_shader_;
	}
	bool GraphicsPipeline::SetRasterizerState(const stdfs::path& rss_name)
	{
		rasterizer_state_ = ResourceManager::GetInst().Find<RasterizerState>(rss_name);
		return (bool)rasterizer_state_;
	}
	bool GraphicsPipeline::SetBlendState(const stdfs::path& bs_name)
	{
		blend_state_ = ResourceManager::GetInst().Find<BlendState>(bs_name);
		return (bool)blend_state_;
	}
	bool GraphicsPipeline::SetDepthStencilState(const stdfs::path& ds_name)
	{
		depth_stencil_state_ = ResourceManager::GetInst().Find<DepthStencilState>(ds_name);
		return (bool)depth_stencil_state_;
	}
	void GraphicsPipeline::Bind(ID3D11DeviceContext* context)
	{
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
	}
}

