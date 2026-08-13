#include "Engine/Core/pch.h"
#include "RenderManager.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Game/Component/Renderer.h>
#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/Graphics/Buffer/ConstantBuffer.h>
#include <Engine/Resource/Graphics/Buffer/StructuredBuffer.h>
#include <Engine/Resource/Graphics/Buffer/VertexBuffer.h>
#include <Engine/Resource/Graphics/Buffer/IndexBuffer.h>
#include <Engine/Resource/Graphics/State/DepthStencilState.h>
#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/GraphicsShaderSet.h>

#include <Engine/Resource/Graphics/Shader/InputLayoutDesc.h>
#include <Engine/Resource/Graphics/Vertex.h>

#include <Engine/HLSL/CppShared/Struct.hlsli>
#include <Engine/HLSL/CppShared/Register.hlsli>

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Core/Debug.h>


namespace engine
{
	RenderManager::RenderManager()
	{
	}

	RenderManager::~RenderManager()
	{
	}
	void RenderManager::Init()
	{
		auto* context = GraphicsDevice::GetInst().GetContext();

		//CONSTANT BUFFERS
		cb_per_pass_ = EntityManager::CreateEntity<ConstantBuffer>();
		cb_per_pass_->Create<PerPass>();

		CreateSamplerStates(context);
		BindPSSamplerStates(context);
		CreateDebugRenderObjects(context);

		present_pass_.Init(context);
	}
	void RenderManager::Render()
	{
		Camera* main_cam = main_cam_.get();
		if (main_cam == nullptr) 
		{ 
			ERROR_MESSAGE("Main Camera 없음");
			return;
		}

		auto* context = GraphicsDevice::GetInst().GetContext();

		//Per Pass ( = Camera )
		PerPass per_pass_data = {};
		per_pass_data.view_mat = main_cam->GetViewMatrix();
		per_pass_data.proj_mat = main_cam->GetProjMatrix();

		cb_per_pass_->Upload(context, per_pass_data);
		cb_per_pass_->Bind(context, ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

		//Render Pass 별 렌더링
		forward_opaque_pass_.Execute(context);

		if (present_pass_.IsSet())
		{
			present_pass_.Execute(context);
		}

		
		//Debug Draw
		DebugDraw(context);
	}
	void RenderManager::FrameEnd()
	{
		//일단은 하지 않음
		//render_queue_.clear();
	}

	void RenderManager::OnScreenSizeChange(uint32 width, uint32 height)
	{
		resolution_width_ = width;
		resolution_height_ = height;
	}
	void RenderManager::OnClearContextStates()
	{
		BindPSSamplerStates(GraphicsDevice::GetInst().GetContext());
	}
	void RenderManager::DebugDraw(ID3D11DeviceContext* context)
	{
		Camera* cam = main_cam_.get();

		if (debug_rect_data_.empty() && debug_circle_data_.empty())
		{
			return;
		}

		//Per Pass ( = Camera )
		PerPass per_pass_data = {};
		per_pass_data.view_mat = cam->GetViewMatrix();
		per_pass_data.proj_mat = cam->GetProjMatrix();

		cb_per_pass_->Upload(context, per_pass_data);
		cb_per_pass_->Bind(context, ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

		//Shader Set Bind
		debug_shader_set_->Bind(context);

		//Debug Draw용 버퍼 - 사이즈 부족 시 확장
		size_t max_size = std::max(debug_rect_data_.size(), debug_circle_data_.size());
		if (max_size > debug_buffer_->GetElementCount())
		{
			//벡터랑 버퍼 확장 타이밍 맞추기 위해 capacity() 사용
			bool result = debug_buffer_->Resize(context, max_size, false);
			ASSERT(result);
		}

		
#pragma region //Debug Rect Draw
		float dt = TimeManager::GetInst().GetDeltaTime();
		if (false == debug_rect_data_.empty())
		{
			debug_buffer_->Upload(context, debug_rect_data_);
			debug_buffer_->BindSRV(context, SLOT_T_PER_INSTANCE, ShaderStage::kVS | ShaderStage::kPS);

			//Mesh Draw
			debug_rect_mesh_->Draw(context, (UINT)debug_rect_data_.size());

			// 순회 돌면서 dt 감소 및 음수가 된 값들은 제거
			
			for (int32 i = (int32)debug_rect_data_.size() - 1; i >= 0; --i)
			{
				debug_rect_data_[i].left_time -= dt;
				if (debug_rect_data_[i].left_time <= 0.0f)
				{
					debug_rect_data_[i] = debug_rect_data_.back();
					debug_rect_data_.pop_back();
				}
			}
		}
#pragma endregion //Debug Rect Draw

#pragma region //Debug Circle Draw
		if (false == debug_circle_data_.empty())
		{
			debug_buffer_->Upload(context, debug_circle_data_);
			debug_buffer_->BindSRV(context, SLOT_T_PER_INSTANCE, ShaderStage::kVS | ShaderStage::kPS);

			//Mesh Draw
			debug_circle_mesh_->Draw(context, (UINT)debug_circle_data_.size());

			// 순회 돌면서 dt 감소 및 음수가 된 값들은 제거
			for (int32 i = (int32)debug_circle_data_.size() - 1; i >= 0; --i)
			{
				debug_circle_data_[i].left_time -= dt;
				if (debug_circle_data_[i].left_time <= 0.0f)
				{
					debug_circle_data_[i] = debug_circle_data_.back();
					debug_circle_data_.pop_back();
				}
			}
		}
#pragma endregion //Debug Circle Draw
	}
	void RenderManager::CreateSamplerStates( ID3D11DeviceContext* context)
	{
		sampler_states_.resize(SLOT_S_END);

		//SAMPLERS
		ComPtr<ID3D11SamplerState> point = {};
		D3D11_SAMPLER_DESC sampler_desc = {};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		// 기본 연산 설정 (Point 필터에서는 크게 의미 없으나 기본값 세팅)
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		// 밉맵 범위 지정 (도트 게임은 보통 밉맵을 1장만 쓰므로 최소 0~ 최대 무한대로 대기)
		sampler_desc.MinLOD = 0.0f;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		
		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateSamplerState(&sampler_desc, point.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
		}
		sampler_states_[SLOT_S_POINT_CLAMP] = point;
	}
	void RenderManager::BindPSSamplerStates(ID3D11DeviceContext* context)
	{
		std::vector<ID3D11SamplerState*> raw_ptrs;
		for (size_t i = 0; i < sampler_states_.size(); ++i)
		{
			raw_ptrs.push_back(sampler_states_[i].Get());
		}

		context->PSSetSamplers(0u, (UINT)raw_ptrs.size(), raw_ptrs.data());
	}
	void RenderManager::CreateDebugRenderObjects(ID3D11DeviceContext* context)
	{
		debug_buffer_ = std::make_unique<StructuredBuffer>();

		StructuredBuffer::BufferFlag flag =
			StructuredBuffer::BufferFlagBitMask::kSRV
			|
			StructuredBuffer::BufferFlagBitMask::kCPUDynamic;

		bool result = debug_buffer_->Create<DebugInstanceData>(flag, 512);
		ASSERT(result);

#pragma region //INPUT LAYOUT DESC
		s_ptr<InputLayoutDesc> input_layout_desc = EntityManager::CreateEntity<InputLayoutDesc>();

		for (const auto& desc : Vertex::Debug::kInputLayoutDescs)
		{
			input_layout_desc->AddLayoutDesc(desc);
		}
#pragma endregion //INPUT LAYOUT DESC

		//Debug DSS
		s_ptr<DepthStencilState> dss = EntityManager::CreateEntity<DepthStencilState>();

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
		if (false == dss->Create(dss_desc))
		{
			ASSERT_RELEASE(false);
		}

#pragma region RECT MESH
		{
			debug_rect_mesh_ = std::make_unique<Mesh>();

			//VERTEX BUFFER
			s_ptr<VertexBuffer> vb = EntityManager::CreateEntity<VertexBuffer>();
			std::vector<Vertex::Debug::Vertex> vertices;
			vertices.resize(4);
			vertices[0].position = { -0.5f, 0.5f, 0.0f };
			vertices[1].position = { 0.5f, 0.5f, 0.0f };
			vertices[2].position = { 0.5f, -0.5f, 0.0f };
			vertices[3].position = { -0.5f, -0.5f, 0.0f };
			vb->Create(vertices);

			//INDEX BUFFER
			s_ptr<IndexBuffer> ib = EntityManager::CreateEntity<IndexBuffer>();
			std::vector<UINT> indices;
			indices.push_back(0u);
			indices.push_back(1u);
			indices.push_back(2u);
			indices.push_back(3u);
			indices.push_back(0u);
			ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

			debug_rect_mesh_->SetBuffers(vb, ib);
		}
#pragma endregion // RECT MESH
#pragma region CIRCLE MESH
		{
			debug_circle_mesh_ = std::make_unique<Mesh>();

			//VERTEX BUFFER & INDEX BUFFER
			s_ptr<VertexBuffer> vb = EntityManager::CreateEntity<VertexBuffer>();
			s_ptr<IndexBuffer> ib = EntityManager::CreateEntity<IndexBuffer>();

			std::vector<Vertex::Debug::Vertex> vertices;
			std::vector<UINT> indices;

			Vertex::Debug::Vertex v;
			//v.position = { 0.0f, 0.0f, 0.0f };	//중심점
			//vertices.push_back(v);

			//32개 (vertex: 32개 index 0 ~ 31)
			for (int32 i = 0; i <= 31; ++i)
			{
				float angle = (float)i / 32.0f * XM_2PI;
				float x = cosf(angle) * 0.5f;
				float y = sinf(angle) * 0.5f;
				vertices.push_back({ {x, y, 0.0f} });
				indices.push_back((UINT)i);
			}
			indices.push_back(0u);	//마지막에 0번으로 돌아가서 닫힌 원 만들기
			vb->Create(vertices);
			ib->Create(indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

			debug_circle_mesh_->SetBuffers(vb, ib);
		}
#pragma endregion // CIRCLE MESH

#pragma region // GRAPHICS SHADER SET
		debug_shader_set_ = std::make_unique<GraphicsShaderSet>();

		debug_shader_set_->SetInstancingSupport(true);
		debug_shader_set_->SetPerInstanceDataStride(sizeof(SpriteInstanceData));
		//Shaders
		debug_shader_set_->SetVertexShader("Shader/Debug_VS.cso"_hash);
		debug_shader_set_->CreateInputLayout(input_layout_desc);
		debug_shader_set_->SetPixelShader("Shader/Debug_PS.cso"_hash);
		debug_shader_set_->SetDepthStencilState(dss);
#pragma endregion // GRAPHICS SHADER SET
	}
}