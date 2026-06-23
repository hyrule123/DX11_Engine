#include "Engine/Core/pch.h"
#include "DefaultRes.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/Graphics/InputLayout.h>
#include <Engine/Resource/Graphics/VertexShader.h>
#include <Engine/Resource/Graphics/PixelShader.h>
#include <Engine/Resource/Graphics/GraphicsPipeline.h>

#include <Engine/Resource/Graphics/Material.h>

#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/VertexBuffer.h>
#include <Engine/Resource/Graphics/IndexBuffer.h>
#include <Engine/Resource/Graphics/Vertex.h>

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
			desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

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
		vertices[0].position = { -0.5f, 0.5f, 0.0f };
		vertices[1].position = { 0.5f, 0.5f, 0.0f };
		vertices[2].position = { 0.5f, -0.5f, 0.0f };
		vertices[3].position = { -0.5f, -0.5f, 0.0f };
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
	void DefaultRes::LoadSpriteRenderObjects()
	{
		auto& resmgr = ResourceManager::GetInst();

		//VERTEX SHADER
		auto vs = resmgr.LoadFromFile<VertexShader>("Shader/Sprite_VS.cso");
		resmgr.SetDefaultResource(vs);

		//INPUT LAYOUT
		auto il = std::make_shared<InputLayout>();
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
			D3D11_INPUT_ELEMENT_DESC desc = {};
			desc.SemanticName = "POSITION";
			desc.SemanticIndex = 0;
			desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			desc.InputSlot = 0;
			desc.AlignedByteOffset = 0;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
			descs.push_back(desc);

			desc.SemanticName = "TEXCOORD";
			desc.SemanticIndex = 0;
			desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			desc.InputSlot = 0;
			desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
			descs.push_back(desc);

			il->Create(descs, vs);

			resmgr.AddResource("SpriteInputLayout", il);
			resmgr.SetDefaultResource(il);
		}

		//PIXEL SHADER
		auto ps = resmgr.LoadFromFile<PixelShader>("Shader/Sprite_PS.cso");
		resmgr.SetDefaultResource(ps);

		//GRAPHICS PIPELINE
		s_ptr<GraphicsPipeline> pipeline = std::make_shared<GraphicsPipeline>();
		pipeline->SetInputLayout(il);
		pipeline->SetVertexShader(vs);
		pipeline->SetPixelShader(ps);
		resmgr.AddResource("SpriteGraphicsPipeline", pipeline);
		resmgr.SetDefaultResource(pipeline);

		//MATERIAL
		s_ptr<Material> material = std::make_shared<Material>();
		material->SetGraphicsPipeline(pipeline);

		//VERTEX BUFFER
		auto vb = std::make_shared<VertexBuffer>();
		std::vector<Vertex2D> vertices;
		vertices.resize(4);
		vertices[0].position = { -0.5f, 0.5f, 0.0f };
		vertices[1].position = { 0.5f, 0.5f, 0.0f };
		vertices[2].position = { 0.5f, -0.5f, 0.0f };
		vertices[3].position = { -0.5f, -0.5f, 0.0f };
		vb->Create(vertices);

		vertices[0].UV = { 0.0f, 0.0f };
		vertices[1].UV = { 1.0f, 0.0f };
		vertices[2].UV = { 1.0f, 1.0f };
		vertices[3].UV = { 0.0f, 1.0f };

		//INDEX BUFFER
		auto ib = std::make_shared<IndexBuffer>();
		std::vector<UINT> indices = { 0, 1, 2, 0, 2, 3 };
		ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//MESH
		auto msh = std::make_shared<Mesh>();
		msh->SetBuffers(vb, ib);

		//최종 등록
		resmgr.AddResource("SpriteMaterial", material);
		resmgr.AddResource("SpriteMesh", msh);

		resmgr.SetDefaultResource(material);
		resmgr.SetDefaultResource(msh);
	}
}