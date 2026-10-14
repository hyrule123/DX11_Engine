#include "Engine/Core/pch.h"
#include "BlitPass.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/GPU/Shader/PixelShader.h>
#include <Engine/Resource/GPU/State/DepthStencilState.h>
#include <Engine/Resource/GPU/Buffer/Texture2D.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/RenderTargetGroup.h>
#include <Engine/Resource/GPU/PipelineState.h>

#include <Engine/Render/RenderView.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/BlitPass/BlitPass.hlsli>

#include <array>

namespace engine
{
	namespace blit_pass
	{
		using Vertex = BlitVSInput;

		constexpr auto kInputLayoutDescs = []()
			{
				std::array<D3D11_INPUT_ELEMENT_DESC, 2> descs = {};

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
				descs[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				descs[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descs[1].InstanceDataStepRate = 0;

				return descs;
			}();

		// 클리어하지 않는 설정 (ClearSettings 기본값은 Color | DepthStencil이므로 명시적으로 비움)
		ClearSettings MakeNoClear()
		{
			ClearSettings settings = {};
			settings.flags = {};
			return settings;
		}
	}

	BlitPass::BlitPass(render_pass::ID id)
		: BlitPass(BlitPass::kClassConcreteName, id)
	{}

	BlitPass::BlitPass(const HashedStringView& concrete_class_name, render_pass::ID id)
		: Super(concrete_class_name, id)
		, mesh_()
		, pipeline_state_()
		, default_pixel_shader_()
		, input_texture_()
		, output_mode_(OutputMode::ViewTarget)
		, output_target_()
		, output_clear_(blit_pass::MakeNoClear())
	{}

	BlitPass::~BlitPass()
	{}

	void BlitPass::Init()
	{
		Super::Init();

		{
			// 전체 화면 쿼드 (NDC 좌표, uv는 좌상단 원점)
			std::unique_ptr<Mesh> mesh = EntityManager::CreateEntity<Mesh>();
			std::vector<blit_pass::Vertex> vertices;
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
			bool result = mesh->CreateVertexBuffer<blit_pass::Vertex>(vertices, bounds);
			ASSERT(result);

			std::vector<uint16> indices = { 0, 1, 2, 0, 2, 3 };
			result = mesh->CreateIndexBuffer(indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ASSERT(result);

			mesh_ = std::move(mesh);
		}

		pipeline_state_ = std::make_unique<PipelineState>();

		bool result = pipeline_state_->SetVertexShader("Shader/BlitPass_VS.cso");
		ASSERT(result);
		pipeline_state_->CreateInputLayout(blit_pass::kInputLayoutDescs);

		result = pipeline_state_->SetPixelShader("Shader/BlitPass_PS.cso");
		ASSERT(result);
		default_pixel_shader_ = pipeline_state_->GetPixelShader();

		pipeline_state_->SetRasterizerState("RSS_Solid_Back");

		// 전체 화면 쿼드가 출력 대상의 깊이 버퍼를 건드리지 않도록 깊이 테스트/쓰기 모두 끔
		std::shared_ptr<DepthStencilState> dss = EntityManager::CreateEntity<DepthStencilState>();
		{
			D3D11_DEPTH_STENCIL_DESC desc = {};
			desc.DepthEnable = FALSE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = FALSE;
			result = dss->Create(desc);
			ASSERT(result);
		}
		pipeline_state_->SetDepthStencilState(dss);
	}

	void BlitPass::Execute(const RenderContext& ctx)
	{
		RenderTargetGroup* output = ResolveOutput(ctx);
		if (output == nullptr)
		{
			return;
		}

		RenderTargetGroup* input_rtg = nullptr;
		if (input_texture_ == nullptr)
		{
			input_rtg = ctx.view.GetTarget();

			// 입력과 출력이 같은 RTG면 할 일이 없음
			// (기본 씬의 Present: View 타깃이 곧 스왑체인)
			if (input_rtg == nullptr || input_rtg == output)
			{
				return;
			}
		}

		// 입력을 SRV로 걸기 전에 OM을 비움.
		// 직전 패스가 같은 텍스처를 RTV로 걸어 둔 채 SRV로 바인딩하면 D3D11이 SRV를 null로 바꿔 버린다.
		ctx.dc->OMSetRenderTargets(0, nullptr, nullptr);

		if (input_texture_)
		{
			input_texture_->BindSRV(ctx.dc, ShaderStage::Pixel, REG_T_SRC_RENDERTARGET.Get());
		}
		else
		{
			input_rtg->BindShaderResourceViews(ctx.dc, ShaderStage::Pixel, REG_T_SRC_RENDERTARGET);
		}

		output->Clear(ctx.dc, output_clear_);
		output->BindOutputMerger(ctx.dc);	// RTV/DSV + 뷰포트

		pipeline_state_->Bind(ctx.dc);
		mesh_->Bind(ctx.dc);
		mesh_->Draw(ctx.dc, { .instance_start = 0, .instance_count = 1, .submesh_index = 0 });

		// 입력 해제 (다음 프레임에 같은 텍스처를 RTV로 바인딩할 수 있도록)
		if (input_texture_)
		{
			ID3D11ShaderResourceView* null_srv = nullptr;
			ctx.dc->PSSetShaderResources(REG_T_SRC_RENDERTARGET.Get(), 1, &null_srv);
		}
		else
		{
			input_rtg->UnBindShaderResourceViews(ctx.dc, ShaderStage::Pixel, REG_T_SRC_RENDERTARGET);
		}
	}

	void BlitPass::SetInputTexture(std::shared_ptr<Texture2D> texture)
	{
		input_texture_ = std::move(texture);
	}

	void BlitPass::SetOutputTarget(std::shared_ptr<RenderTargetGroup> rtg, const ClearSettings& clear)
	{
		output_mode_ = rtg ? OutputMode::RenderTargetGroup : OutputMode::ViewTarget;
		output_target_ = std::move(rtg);
		output_clear_ = clear;
	}

	void BlitPass::SetOutputSwapChain(const ClearSettings& clear)
	{
		output_mode_ = OutputMode::SwapChain;
		output_target_ = nullptr;
		output_clear_ = clear;
	}

	void BlitPass::SetPixelShader(std::shared_ptr<PixelShader> pixel_shader)
	{
		ASSERT_F(pipeline_state_ != nullptr, "BlitPass: Init 이전에는 PS를 교체할 수 없습니다.");
		pipeline_state_->SetPixelShader(pixel_shader ? std::move(pixel_shader) : default_pixel_shader_);
	}

	RenderTargetGroup* BlitPass::ResolveOutput(const RenderContext& ctx) const
	{
		switch (output_mode_)
		{
		case OutputMode::ViewTarget:
		{
			return ctx.view.GetTarget();
		}
		case OutputMode::SwapChain:
		{
			return GraphicsDevice::GetInst().GetSwapChainRenderTargetGroup();
		}
		case OutputMode::RenderTargetGroup:
		{
			return output_target_.get();
		}
		default:
		{
			ASSERT_F(false, "BlitPass: 알 수 없는 OutputMode");
			return nullptr;
		}
		}
	}
}