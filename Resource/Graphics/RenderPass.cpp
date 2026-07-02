#include "Engine/Core/pch.h"
#include "RenderPass.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/Shader/InputLayout.h>
#include <Engine/Resource/Graphics/Shader/VertexShader.h>
#include <Engine/Resource/Graphics/State/RasterizerState.h>
#include <Engine/Resource/Graphics/Shader/PixelShader.h>
#include <Engine/Resource/Graphics/State/DepthStencilState.h>
#include <Engine/Resource/Graphics/State/BlendState.h>
#include <Engine/Resource/Graphics/RenderTargetGroup.h>


namespace engine
{
	RenderPass::RenderPass()
		: Resource(STRINGIFY(RenderPass))
	{
	}

	RenderPass::~RenderPass()
	{
	}
	bool RenderPass::SetInputLayout(const stdfs::path& layout_name)
	{
		input_layout_ = ResourceManager::GetInst().Find<InputLayout>(layout_name);
		return (bool)input_layout_;
	}
	bool RenderPass::SetInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& il_desc, const stdfs::path& vs_path)
	{
		auto device = GraphicsDevice::GetInst().GetDevice();
		input_layout_ = std::make_shared<InputLayout>();
		return input_layout_->Create(device.Get(), il_desc, vs_path);
	}
	bool RenderPass::SetVertexShader(const stdfs::path& vs_name)
	{
		vertex_shader_ = ResourceManager::GetInst().LoadFromFile<VertexShader>(vs_name);
		return (bool)vertex_shader_;
	}
	bool RenderPass::SetPixelShader(const stdfs::path& ps_name)
	{
		pixel_shader_ = ResourceManager::GetInst().LoadFromFile<PixelShader>(ps_name);
		return (bool)pixel_shader_;
	}
	bool RenderPass::SetRasterizerState(const stdfs::path& rss_name)
	{
		rasterizer_state_ = ResourceManager::GetInst().Find<RasterizerState>(rss_name);
		return (bool)rasterizer_state_;
	}
	bool RenderPass::SetBlendState(const stdfs::path& bs_name)
	{
		blend_state_ = ResourceManager::GetInst().Find<BlendState>(bs_name);
		return (bool)blend_state_;
	}
	bool RenderPass::SetDepthStencilState(const stdfs::path& ds_name)
	{
		depth_stencil_state_ = ResourceManager::GetInst().Find<DepthStencilState>(ds_name);
		return (bool)depth_stencil_state_;
	}
	bool RenderPass::SetRenderTargetGroup(const stdfs::path& rtg_name)
	{
		render_target_group_ = ResourceManager::GetInst().Find<RenderTargetGroup>(rtg_name);
		return (bool)render_target_group_;
	}
	void RenderPass::Bind(ID3D11DeviceContext* context)
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

		if (render_target_group_) { render_target_group_->BindOutputMerger(context); }
	}
}

