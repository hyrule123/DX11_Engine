#include "Engine/Core/pch.h"
#include "PresentPass.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/Shader/InputLayoutDesc.h>
#include <Engine/Resource/Graphics/Shader/VertexShader.h>
#include <Engine/Resource/Graphics/Shader/PixelShader.h>
#include <Engine/Resource/Graphics/State/RasterizerState.h>
#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/Buffer/VertexBuffer.h>
#include <Engine/Resource/Graphics/Buffer/IndexBuffer.h>

#include <Engine/Resource/Graphics/RenderTargetGroup.h>
#include <Engine/Resource/Graphics/GraphicsShaderSet.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/Present/Present.hlsli>

namespace engine
{
	namespace Present_Pass
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
		: Super(PresentPass::kClassConcreteName, RenderPassOrder::kPresent)
	{}
	PresentPass::~PresentPass()
	{}
	void PresentPass::Init(ID3D11DeviceContext* context)
	{
		ResourceManager& res_mgr = ResourceManager::GetInst();
		const stdfs::path& res_dir = res_mgr.GetResourceDir();

		{
			mesh_ = std::make_unique<Mesh>();

			//VERTEX BUFFER
			auto vb = std::make_shared<VertexBuffer>();
			std::vector<Present_Pass::Vertex> vertices;
			vertices.resize(4);
			vertices[0].position = { -1.0f, 1.0f, 0.5f };
			vertices[1].position = { 1.0f, 1.0f, 0.5f };
			vertices[2].position = { 1.0f, -1.0f, 0.5f };
			vertices[3].position = { -1.0f, -1.0f, 0.5f };

			vertices[0].UV = { 0.0f, 0.0f };
			vertices[1].UV = { 1.0f, 0.0f };
			vertices[2].UV = { 1.0f, 1.0f };
			vertices[3].UV = { 0.0f, 1.0f };

			bool result = vb->Create(vertices);
			ASSERT(result);

			//INDEX BUFFER
			auto ib = std::make_shared<IndexBuffer>();
			std::vector<UINT> indices = { 0, 1, 2, 0, 2, 3 };
			result = ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			ASSERT(result);

			mesh_->SetBuffers(vb, ib);
		}

		shader_set_ = std::make_unique<GraphicsShaderSet>();
		shader_set_->SetPerInstanceDataStride(sizeof(PresentVSInput));
		
		s_ptr<VertexShader> vs = std::make_shared<VertexShader>();
		bool result = vs->LoadFromFile(res_dir / L"Shader/Present_VS.cso");
		ASSERT(result);
		shader_set_->SetVertexShader(vs);

		s_ptr<InputLayoutDesc> layout_desc = std::make_shared<InputLayoutDesc>();
		for (const auto& desc : Present_Pass::kInputLayoutDescs)
		{
			layout_desc->AddLayoutDesc(desc);
		}
		result = shader_set_->CreateInputLayout(layout_desc);
		ASSERT(result);

		s_ptr<PixelShader> ps = std::make_shared<PixelShader>();
		result = ps->LoadFromFile(res_dir / L"Shader/Present_PS.cso");
		ASSERT(result);
		shader_set_->SetPixelShader(ps);

		shader_set_->SetRasterizerState("RSS_Solid_Back"_hash);
		shader_set_->SetDepthStencilState("DSS_Default"_hash);
	}
	void PresentPass::Execute(ID3D11DeviceContext * context)
	{
		if (!src_render_target_) { return; }

		src_render_target_->UnBindOutputMerger(context);

		// 이전 렌더패스에서 렌더링된 결과를 PresentPass에서 사용하기 위해 SRV로 바인딩
		src_render_target_->BindShaderResourceViews(context, ShaderStage::kPS);

		// 전용 ShderSet 바인딩
		shader_set_->Bind(context);

		// 스왑체인 렌더타겟을 바인딩
		GraphicsDevice::GetInst().BindSwapChainRTV();

		// 전용 Mesh를 사용하여 화면에 렌더링
		mesh_->Draw(context, 1);

		src_render_target_->UnBindShaderResourceViews(context);
		src_render_target_->ClearRenderTargetView(context, { 0.0f, 0.0f, 0.0f, 0.0f });
		src_render_target_->ClearDepthStencilView(context, 1.0f, 0);
	}
}