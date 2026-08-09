#include "Engine/Core/pch.h"
#include "DefaultRes.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/GraphicsShaderSet.h>
#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/Buffer/VertexBuffer.h>
#include <Engine/Resource/Graphics/Buffer/IndexBuffer.h>
#include <Engine/Resource/Graphics/Vertex.h>
#include <Engine/Resource/Graphics/State/RasterizerState.h>
#include <Engine/Resource/Graphics/State/DepthStencilState.h>
#include <Engine/Resource/Graphics/Shader/InputLayoutDesc.h>

#include <Engine/Core/Math.h>
#include <Engine/Core/Debug.h>



#include <Engine/HLSL/Sprite/Sprite.hlsli>


namespace engine
{
	void DefaultRes::LoadDefaultResources()
	{
		LoadDefaultDepthStencilStates();
		LoadDefaultRasterizerStates();
		LoadDefaultBlendStates();
		LoadSpriteRenderObjects();
	}

	void DefaultRes::LoadDefaultRasterizerStates()
	{
		auto& res_mgr = ResourceManager::GetInst();
		auto device = GraphicsDevice::GetInst().GetDevice();
		{
			s_ptr<RasterizerState> rss = EntityManager::CreateEntity<RasterizerState>();
			res_mgr.AddResource("RSS_Solid_Back"_hash, rss);
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

			if (false == rss->Create(rs_desc))
			{
				ASSERT_RELEASE(false);
			}
		}

		{
			s_ptr<RasterizerState> rss_wireframe = EntityManager::CreateEntity<RasterizerState>();
			res_mgr.AddResource("RSS_WireFrame"_hash, rss_wireframe);
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

			if (!rss_wireframe->Create(rsDesc))
			{
				ASSERT_RELEASE(false);
			}
		}
	}
	void DefaultRes::LoadDefaultDepthStencilStates()
	{
		auto device = GraphicsDevice::GetInst().GetDevice();

		{//Default DSS
			s_ptr<DepthStencilState> dss = EntityManager::CreateEntity<DepthStencilState>();
			ResourceManager::GetInst().AddResource("DSS_Default"_hash, dss);
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
			if (false == dss->Create(dss_desc))
			{
				ASSERT_RELEASE(false);
			}
		}
	}
	void DefaultRes::LoadDefaultBlendStates()
	{
		
	}


	void DefaultRes::LoadSpriteRenderObjects()
	{
		auto& resmgr = ResourceManager::GetInst();
		auto device = GraphicsDevice::GetInst().GetDevice();

#pragma region //INPUT LAYOUT DESC
		s_ptr<InputLayoutDesc> input_layout_desc = EntityManager::CreateEntity<InputLayoutDesc>();
		for (const auto& desc : Vertex::Standard2D::kInputLayoutDescs)
		{
			input_layout_desc->AddLayoutDesc(desc);
		}
		resmgr.AddResource("InputLayoutDesc_Standard2D"_hash, input_layout_desc);
		resmgr.SetDefaultResource(input_layout_desc);
#pragma endregion //INPUT LAYOUT DESC

#pragma region //MESH
		{
			s_ptr<Mesh> msh = EntityManager::CreateEntity<Mesh>();
			resmgr.AddResource("Mesh_Standard2D_Rect"_hash, msh);
			resmgr.SetDefaultResource(msh);

			//VERTEX BUFFER
			s_ptr<VertexBuffer> vb = EntityManager::CreateEntity<VertexBuffer>();
			std::vector<Vertex::Standard2D::Vertex> vertices;
			vertices.resize(4);
			vertices[0].position = { -0.5f, 0.5f, 0.0f };
			vertices[1].position = { 0.5f, 0.5f, 0.0f };
			vertices[2].position = { 0.5f, -0.5f, 0.0f };
			vertices[3].position = { -0.5f, -0.5f, 0.0f };

			vertices[0].UV = { 0.0f, 0.0f };
			vertices[1].UV = { 1.0f, 0.0f };
			vertices[2].UV = { 1.0f, 1.0f };
			vertices[3].UV = { 0.0f, 1.0f };

			vb->Create(vertices);

			//INDEX BUFFER
			s_ptr<IndexBuffer> ib = EntityManager::CreateEntity<IndexBuffer>();
			std::vector<UINT> indices = { 0, 1, 2, 0, 2, 3 };
			ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			msh->SetBuffers(vb, ib);
		}
#pragma endregion //MESH
		
#pragma region //GRAPHICS SHADER SET
		s_ptr<GraphicsShaderSet> shaderset = EntityManager::CreateEntity<GraphicsShaderSet>();
		
		shaderset->SetInstancingSupport(true);
		shaderset->SetPerInstanceDataStride(sizeof(SpriteInstanceData));
		shaderset->SetVertexShader("Shader/Sprite_VS.cso"_hash);
		shaderset->CreateInputLayout("InputLayoutDesc_Standard2D"_hash);
		shaderset->SetPixelShader("Shader/Sprite_PS.cso"_hash);
		shaderset->SetDepthStencilState("DSS_Default"_hash);
		shaderset->SetRasterizerState("RSS_Solid_Back"_hash);

		resmgr.AddResource("GraphicsShaderSet_Sprite"_hash, shaderset);
		resmgr.SetDefaultResource(shaderset);

		//MATERIAL
		s_ptr<Material> material = EntityManager::CreateEntity<Material>();
		material->SetShaderSet(shaderset, RenderPassOrder::kForwardOpaque);
		resmgr.AddResource("Material_Sprite"_hash, material);
		resmgr.SetDefaultResource(material);
	}
}