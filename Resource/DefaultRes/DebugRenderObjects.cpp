#include "Engine/Core/pch.h"
#include "DefaultRes.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/GraphicsPipeline/InputLayout.h>
#include <Engine/Resource/GraphicsPipeline/VertexShader.h>
#include <Engine/Resource/GraphicsPipeline/PixelShader.h>
#include <Engine/Resource/GraphicsPipeline/GraphicsPipeline.h>

#include <Engine/Resource/Material/Material.h>

#include <Engine/Resource/Mesh/Mesh.h>
#include <Engine/Resource/Mesh/VertexBuffer.h>
#include <Engine/Resource/Mesh/IndexBuffer.h>
#include <Engine/Resource/Mesh/Vertex.h>

#include <Engine/Core/Math.h>

namespace engine
{
	void DefaultRes::LoadDebugRenderObjects()
	{
		auto& resmgr = ResourceManager::GetInst();

		//VERTEX SHADER
		auto vs = resmgr.LoadFromFile<VertexShader>("Shader/Debug_VS.cso");
		resmgr.SetDefaultResource(vs);

		//INPUT LAYOUT
		auto il = std::make_shared<InputLayout>();
		{
			D3D11_INPUT_ELEMENT_DESC desc = {};

			//VS의 입력 구조체 매개변수 이름과 일치해야 함
			desc.SemanticName = "POSITION";

			//POSITION0을 의미 (뒤에 숫자가 없을 때 기본값 0)
			desc.SemanticIndex = 0;

			//데이터 포맷
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

			//InputSlot: 0번 정점 버퍼 슬롯을 사용합니다.
			desc.InputSlot = 0;

			//AlignedByteOffset: 구조체 시작점으로부터 몇 번째부터 읽어야 하는지
			desc.AlignedByteOffset = 0;

			//정점별로 데이터를 보낼것인지 인스턴스별로 보낼것인지
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

			//인스턴스 별로 보낼 때 인스턴스 별로 몇단계씩 올릴 것인지
			//e.g) 2 -> 0, 2, 4, 8, ...
			desc.InstanceDataStepRate = 0;

			std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
			descs.push_back(desc);

			il->Create(descs, vs);

			resmgr.AddResource("Debug_InputLayout", il);
			resmgr.SetDefaultResource(il);
		}

		//PIXEL SHADER
		auto ps = resmgr.LoadFromFile<PixelShader>("Shader/Debug_PS.cso");
		resmgr.SetDefaultResource(ps);

		//GRAPHICS PIPELINE
		s_ptr<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>();
		pipeline->SetInputLayout(il);
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		resmgr.AddResource("Debug_GraphicsPipeline", pipeline);
		resmgr.SetDefaultResource(pipeline);

		s_ptr<Material> material = std::make_shared<Material>();
		material->SetGraphicsPipeline(pipeline);


		//VERTEX BUFFER
		auto vb = std::make_shared<VertexBuffer>();
		std::vector<VertexDebug> vertices;
		vertices.resize(4);
		vertices[0].position = { -0.5f, 0.5f, 0.5f, 1.f };
		vertices[1].position = { 0.5f, 0.5f, 0.5f, 1.f };
		vertices[2].position = { 0.5f, -0.5f, 0.5f, 1.f };
		vertices[3].position = { -0.5f, -0.5f, 0.5f, 1.f };
		vb->Create(vertices);

		//INDEX BUFFER
		auto ib = std::make_shared<IndexBuffer>();
		std::vector<UINT> indices;
		indices.push_back(0u);
		indices.push_back(1u);
		indices.push_back(2u);
		indices.push_back(3u);
		indices.push_back(0u);
		ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		//MESH
		auto msh = std::make_shared<Mesh>();
		msh->SetBuffers(vb, ib);

		//최종 등록
		resmgr.AddResource("Debug_Material", material);
		resmgr.AddResource("Debug_Mesh", msh);
		
		resmgr.SetDefaultResource(material);
		resmgr.SetDefaultResource(msh);
	}
}