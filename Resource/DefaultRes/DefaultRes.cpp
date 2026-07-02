#include "Engine/Core/pch.h"
#include "DefaultRes.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/Graphics/Shader/InputLayout.h>
#include <Engine/Resource/Graphics/Shader/VertexShader.h>
#include <Engine/Resource/Graphics/Shader/PixelShader.h>
#include <Engine/Resource/Graphics/RenderPass.h>

#include <Engine/Resource/Graphics/Material.h>

#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/Buffer/VertexBuffer.h>
#include <Engine/Resource/Graphics/Buffer/IndexBuffer.h>
#include <Engine/Resource/Graphics/Vertex.h>
#include <Engine/Resource/Graphics/State/RasterizerState.h>
#include <Engine/Resource/Graphics/State/BlendState.h>
#include <Engine/Resource/Graphics/State/DepthStencilState.h>

#include <Engine/Core/Math.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	void DefaultRes::LoadDefaultResources()
	{
		LoadDefaultDepthStencilStates();
		LoadDefaultRasterizerStates();
		LoadDefaultBlendStates();

		LoadDebugRenderObjects();
		LoadSpriteRenderObjects();
	}
	void DefaultRes::LoadDefaultRasterizerStates()
	{
		auto& res_mgr = ResourceManager::GetInst();
		auto device = GraphicsDevice::GetInst().GetDevice();
		{
			s_ptr<RasterizerState> rss = std::make_shared<RasterizerState>();
			res_mgr.AddResource("RSS_Solid_Back", rss);
			res_mgr.SetDefaultResource(rss);

			// 구조체 초기화
			D3D11_RASTERIZER_DESC rs_desc = {};

			// --------------------------------------------------------
			// 1. 기본 상태 (Default: Solid, Cull Back)
			// --------------------------------------------------------
			// 대부분의 불투명 3D 모델(캐릭터, 배경 등)을 그릴 때 사용
			rs_desc.FillMode = D3D11_FILL_SOLID;               // 내부를 색상으로 채움
			rs_desc.CullMode = D3D11_CULL_BACK;                // 백페이스 컬링(기본값: BACK)
			rs_desc.FrontCounterClockwise = false;             // Winding Order 설정(기본: CW)
			rs_desc.DepthClipEnable = false;

			if (false == rss->Create(device.Get(), rs_desc))
			{
				ASSERT_RELEASE(false);
			}
		}

		{
			s_ptr<RasterizerState> rss_wireframe = std::make_shared<RasterizerState>();
			res_mgr.AddResource("RSS_WireFrame", rss_wireframe);
			res_mgr.SetDefaultResource(rss_wireframe);

			// 구조체 초기화
			D3D11_RASTERIZER_DESC rsDesc = {};

			// --------------------------------------------------------
			// 2. 와이어프레임 상태 (Wireframe)
			// --------------------------------------------------------
			// 충돌체 디버깅, 메쉬 토폴로지 확인, 에디터 모드 등에서 사용
			// 최적화 때문에 안 나오는 일은 없어야 하므로 무조건 그리는 설정으로 지정
			rsDesc.FillMode = D3D11_FILL_WIREFRAME;     // 선으로만 그림
			rsDesc.CullMode = D3D11_CULL_NONE;			// 백페이스 컬링 X
			rsDesc.FrontCounterClockwise = false;
			rsDesc.DepthClipEnable = false;				//Depth Clip OFF

			if (!rss_wireframe->Create(device.Get(), rsDesc))
			{
				ASSERT_RELEASE(false);
			}
		}
	}
	void DefaultRes::LoadDefaultDepthStencilStates()
	{
		auto device = GraphicsDevice::GetInst().GetDevice();

		{//Debug DSS
			s_ptr<DepthStencilState> dss = std::make_shared<DepthStencilState>();
			ResourceManager::GetInst().AddResource("DSS_Debug", dss);
			ResourceManager::GetInst().SetDefaultResource(dss);

			// 1. Depth Stencil Desc 구조체 선언 및 초기화
			D3D11_DEPTH_STENCIL_DESC dss_desc = {};

			// ------------------------------------------------------------------
			// 1. 깊이(Depth) 테스트 설정
			// ------------------------------------------------------------------
			dss_desc.DepthEnable = FALSE;                           // 깊이 테스트 OFF

			// ------------------------------------------------------------------
			// 2. 스텐실(Stencil) 테스트 설정 (일반적인 기본 렌더링에서는 끕니다)
			// ------------------------------------------------------------------
			dss_desc.StencilEnable = FALSE;                        // 스텐실 테스트를 끕니다.

			// 2. State 객체 생성
			if (false == dss->Create(device.Get(), dss_desc))
			{
				ASSERT_RELEASE(false);
			}
		}

		{//Default DSS
			s_ptr<DepthStencilState> dss = std::make_shared<DepthStencilState>();
			ResourceManager::GetInst().AddResource("DSS_Default", dss);
			ResourceManager::GetInst().SetDefaultResource(dss);

			// 1. Depth Stencil Desc 구조체 선언 및 초기화
			D3D11_DEPTH_STENCIL_DESC dss_desc = {};

			// ------------------------------------------------------------------
			// 1. 깊이(Depth) 테스트 설정
			// ------------------------------------------------------------------
			dss_desc.DepthEnable = TRUE;                           // 깊이 테스트를 켭니다.
			dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  // 가려지지 않은 물체의 깊이 값을 깊이 버퍼에 기록합니다. (불투명 오브젝트용)
			dss_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;   // ReversedZ

			// ------------------------------------------------------------------
			// 2. 스텐실(Stencil) 테스트 설정 (일반적인 기본 렌더링에서는 끕니다)
			// ------------------------------------------------------------------
			dss_desc.StencilEnable = FALSE;                        // 스텐실 테스트를 끕니다.

			// 2. State 객체 생성
			if (false == dss->Create(device.Get(), dss_desc))
			{
				ASSERT_RELEASE(false);
			}
		}
	}
	void DefaultRes::LoadDefaultBlendStates()
	{
		
	}
	void DefaultRes::LoadDebugRenderObjects()
	{
		auto& resmgr = ResourceManager::GetInst();
		auto device = GraphicsDevice::GetInst().GetDevice();

		//GRAPHICS PIPELINE
		s_ptr<RenderPass> renderpass = std::make_shared<RenderPass>();
		resmgr.AddResource("Debug_GraphicsPipeline", renderpass);
		resmgr.SetDefaultResource(renderpass);

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

			if (false == renderpass->SetInputLayout(descs, "Shader/Debug_VS.cso"))
			{
				ASSERT_RELEASE(false);
			}
		}

		//Shaders
		renderpass->SetVertexShader("Shader/Debug_VS.cso");
		renderpass->SetPixelShader("Shader/Debug_PS.cso");
		renderpass->SetDepthStencilState("DSS_Debug");

		///////////// MATERIAL ////////////////
		s_ptr<Material> material = std::make_shared<Material>();
		material->SetGraphicsPipeline(renderpass);
		resmgr.AddResource("Debug_Material", material);
		resmgr.SetDefaultResource(material);


		/////////////  MESH  //////////////
		//VERTEX BUFFER
		auto vb = std::make_shared<VertexBuffer>();
		std::vector<VertexDebug> vertices;
		vertices.resize(4);
		vertices[0].position = { -0.5f, 0.5f, 0.0f };
		vertices[1].position = { 0.5f, 0.5f, 0.0f };
		vertices[2].position = { 0.5f, -0.5f, 0.0f };
		vertices[3].position = { -0.5f, -0.5f, 0.0f };
		vb->Create(device.Get(), vertices);

		//INDEX BUFFER
		auto ib = std::make_shared<IndexBuffer>();
		std::vector<UINT> indices;
		indices.push_back(0u);
		indices.push_back(1u);
		indices.push_back(2u);
		indices.push_back(3u);
		indices.push_back(0u);
		ib->Create(device.Get(), indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		//MESH
		auto msh = std::make_shared<Mesh>();
		msh->SetBuffers(vb, ib);
		resmgr.AddResource("Debug_RectMesh", msh);
		resmgr.SetDefaultResource(msh);

		//RenderTargetGroup
		auto rtg = GraphicsDevice::GetInst().GetSwapChainRenderTargetGroup();

	}
	void DefaultRes::LoadSpriteRenderObjects()
	{
		auto& resmgr = ResourceManager::GetInst();
		auto device = GraphicsDevice::GetInst().GetDevice();

		auto pipeline = std::make_shared<RenderPass>();

		//INPUT LAYOUT
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
			//참고: 같은 InputSlot 사용 시 D3D11_APPEND_ALIGNED_ELEMENT로 자동 정렬 가능
			desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
			descs.push_back(desc);

			pipeline->SetInputLayout(descs, "Shader/Sprite_VS.cso");
		}


		pipeline->SetVertexShader("Shader/Sprite_VS.cso");
		pipeline->SetPixelShader("Shader/Sprite_PS.cso");
		pipeline->SetDepthStencilState("DSS_Default");

		resmgr.AddResource("Sprite_GraphicsPipeline", pipeline);
		resmgr.SetDefaultResource(pipeline);

		//MATERIAL
		s_ptr<Material> material = std::make_shared<Material>();
		material->SetGraphicsPipeline(pipeline);
		resmgr.AddResource("Sprite_Material", material);
		resmgr.SetDefaultResource(material);

		//MESH
		auto msh = std::make_shared<Mesh>();
		resmgr.AddResource("Sprite_Mesh", msh);
		resmgr.SetDefaultResource(msh);

		//VERTEX BUFFER
		auto vb = std::make_shared<VertexBuffer>();
		std::vector<Vertex2D> vertices;
		vertices.resize(4);
		vertices[0].position = { -0.5f, 0.5f, 0.0f };
		vertices[1].position = { 0.5f, 0.5f, 0.0f };
		vertices[2].position = { 0.5f, -0.5f, 0.0f };
		vertices[3].position = { -0.5f, -0.5f, 0.0f };

		vertices[0].UV = { 0.0f, 0.0f };
		vertices[1].UV = { 1.0f, 0.0f };
		vertices[2].UV = { 1.0f, 1.0f };
		vertices[3].UV = { 0.0f, 1.0f };

		vb->Create(device.Get(), vertices);

		//INDEX BUFFER
		auto ib = std::make_shared<IndexBuffer>();
		std::vector<UINT> indices = { 0, 1, 2, 0, 2, 3 };
		ib->Create(device.Get(), indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		msh->SetBuffers(vb, ib);
	}
}