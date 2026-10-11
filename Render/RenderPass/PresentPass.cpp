#include "Engine/Core/pch.h"
#include "PresentPass.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/Shader/InputLayout.h>
#include <Engine/Resource/GPU/Shader/VertexShader.h>
#include <Engine/Resource/GPU/Shader/PixelShader.h>
#include <Engine/Resource/GPU/State/RasterizerState.h>
#include <Engine/Resource/GPU/Mesh.h>

#include <Engine/Resource/GPU/RenderTargetGroup.h>
#include <Engine/Resource/GPU/PipelineState.h>

#include <Engine/Render/RenderView.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/PresentPass/PresentPass.hlsli>

#include <array>

namespace engine
{
	namespace present_pass
	{
		using Vertex = PresentVSInput;

		constexpr auto kInputLayoutDescs = []() {
			std::array<D3D11_INPUT_ELEMENT_DESC, 2> descs;

			descs[0].SemanticName = "POSITION";
			descs[0].SemanticIndex = 0;
			descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			descs[0].InputSlot = 0;
			descs[0].AlignedByteOffset = 0;
			descs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descs[0].InstanceDataStepRate = 0;

			descs[1].SemanticName = "TEXCOORD";
			descs[1].SemanticIndex = 0;
			descs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
			descs[1].InputSlot = 0;
			//참고: 같은 InputSlot 사용 시 D3D11_APPEND_ALIGNED_ELEMENT로 자동 정렬 가능
			descs[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			descs[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descs[1].InstanceDataStepRate = 0;

			return descs;
			}();
	}

	PresentPass::PresentPass()
		: Super(PresentPass::kClassConcreteName, render_pass::ID::Present)
	{
	}
	PresentPass::~PresentPass()
	{}

	void PresentPass::Init()
	{
		Super::Init();

		ResourceManager& res_mgr = ResourceManager::GetInst();
		const std::filesystem::path& res_dir = res_mgr.GetResourceDir();

		{
			//VERTEX BUFFER
			std::unique_ptr<Mesh> mesh = EntityManager::CreateEntity<Mesh>();
			std::vector<present_pass::Vertex> vertices;
			vertices.resize(4);
			vertices[0].position = { -1.0f, 1.0f, 0.5f };
			vertices[1].position = { 1.0f, 1.0f, 0.5f };
			vertices[2].position = { 1.0f, -1.0f, 0.5f };
			vertices[3].position = { -1.0f, -1.0f, 0.5f };

			vertices[0].uv = { 0.0f, 0.0f };
			vertices[1].uv = { 1.0f, 0.0f };
			vertices[2].uv = { 1.0f, 1.0f };
			vertices[3].uv = { 0.0f, 1.0f };

			AABB3D bounds;
			for (const auto& v : vertices)
			{
				bounds.Encapsulate(v.position);
			}
			bool result = mesh->CreateVertexBuffer<present_pass::Vertex>(vertices, bounds);
			ASSERT(result);

			//INDEX BUFFER
			std::vector<uint16> indices = { 0, 1, 2, 0, 2, 3 };
			result = mesh->CreateIndexBuffer(indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ASSERT(result);

			mesh_ = std::move(mesh);
		}

		shader_set_ = std::make_unique<PipelineState>();
		shader_set_->SetPerInstanceDataStride(sizeof(PresentVSInput));
		shader_set_->SetInstancingSupport(false);

		std::shared_ptr<VertexShader> vs = EntityManager::CreateEntity<VertexShader>();
		bool result = vs->LoadFromFile(res_dir / L"Shader/PresentPass_VS.cso");
		ASSERT(result);
		shader_set_->SetVertexShader(vs);

		std::shared_ptr<PixelShader> ps = EntityManager::CreateEntity<PixelShader>();
		result = ps->LoadFromFile(res_dir / L"Shader/PresentPass_PS.cso");
		ASSERT(result);
		shader_set_->SetPixelShader(ps);

		shader_set_->SetRasterizerState("RSS_Solid_Back");
		shader_set_->SetDepthStencilState("DSS_Default");

		shader_set_->CreateInputLayout(present_pass::kInputLayoutDescs);

		clear_settings_.flags = ClearTarget::Color | ClearTarget::DepthStencil;
		clear_settings_.color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clear_settings_.depth = 1.0f;
		clear_settings_.stencil = 0;
	}

	void PresentPass::Execute(const RenderContext& ctx)
	{
		RenderTargetGroup* src = ctx.view.GetTarget();
		RenderTargetGroup* swap_chain = GraphicsDevice::GetInst().GetSwapChainRenderTargetGroup();

		if (src == nullptr || src == swap_chain)
		{
			// 이미 스왑체인 렌더타겟에 렌더링된 경우, PresentPass는 아무것도 하지 않음
			return;
		}

		src->UnBindOutputMerger(ctx.dc);

		// 이전 렌더패스에서 렌더링된 결과를 PresentPass에서 사용하기 위해 SRV로 바인딩
		src->BindShaderResourceViews(ctx.dc, ShaderStage::Pixel, REG_T_SRC_RENDERTARGET);

		// 전용 ShderSet 바인딩
		shader_set_->Bind(ctx.dc);

		swap_chain->Clear(ctx.dc, clear_settings_);

		// 스왑체인 렌더타겟을 바인딩
		GraphicsDevice::GetInst().BindSwapChainRTV();

		mesh_->Bind(ctx.dc);

		// 전용 Mesh를 사용하여 화면에 렌더링
		mesh_->Draw(ctx.dc, {.instance_start = 0, .instance_count = 1, .submesh_index = 0});

		src->UnBindShaderResourceViews(ctx.dc, ShaderStage::Pixel, REG_T_SRC_RENDERTARGET);
	}
}