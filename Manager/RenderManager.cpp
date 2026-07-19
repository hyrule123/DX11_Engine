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
		auto device = GraphicsDevice::GetInst().GetDevice();
		auto context = GraphicsDevice::GetInst().GetContext();

		//CONSTANT BUFFERS
		cb_per_pass_ = std::make_shared<ConstantBuffer>();
		cb_per_pass_->Create<PerPass>(device.Get());

		CreateSamplerStates(device.Get(), context.Get());
		BindPSSamplerStates(context.Get());
		CreateDebugRenderObjects(device.Get(), context.Get());

		present_pass_.Init(device.Get(), context.Get());
	}
	void RenderManager::Render()
	{
		if (main_cam_.expired()) 
		{ 
			ERROR_MESSAGE("Main Camera 없음");
			return; 
		}

		auto device = GraphicsDevice::GetInst().GetDevice();
		auto context = GraphicsDevice::GetInst().GetContext();

		auto cam = main_cam_.lock();

		//Per Pass ( = Camera )
		PerPass per_pass_data = {};
		per_pass_data.view_mat = cam->GetViewMatrix();
		per_pass_data.proj_mat = cam->GetProjMatrix();

		cb_per_pass_->Upload(context.Get(), per_pass_data);
		cb_per_pass_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

		//Render Pass 별 렌더링
		forward_opaque_pass_.Execute(device.Get(), context.Get());

		if (present_pass_.IsSet())
		{
			present_pass_.Execute(device.Get(), context.Get());
		}

		//Debug Draw
		if (!debug_rect_data_.empty())
		{
			//Per Pass ( = Camera )
			PerPass per_pass_data = {};
			per_pass_data.view_mat = cam->GetViewMatrix();
			per_pass_data.proj_mat = cam->GetProjMatrix();

			cb_per_pass_->Upload(context.Get(), per_pass_data);
			cb_per_pass_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

			//Debug Draw용 버퍼 - 사이즈 부족 시 확장
			if (debug_rect_data_.size() > debug_buffer_->GetElementCount())
			{
				//벡터랑 버퍼 확장 타이밍 맞추기 위해 capacity() 사용
				bool result = debug_buffer_->Resize(device.Get(), context.Get(), debug_rect_data_.capacity(), false);
				ASSERT(result);
			}
			debug_buffer_->Upload(context.Get(), debug_rect_data_);
			debug_buffer_->BindSRV(context.Get(), SLOT_T_PER_INSTANCE, ShaderStage::kVS | ShaderStage::kPS);

			//Shader Set Bind
			debug_shader_set_->Bind(context.Get());

			//Mesh Draw
			debug_rect_mesh_->Draw(context.Get(), (UINT)debug_rect_data_.size());

			// 순회 돌면서 dt 감소 및 음수가 된 값들은 제거
			float dt = TimeManager::GetInst().DeltaTime();
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
	}
	void RenderManager::FrameEnd()
	{
		//일단은 하지 않음
		//render_queue_.clear();
	}
	void RenderManager::SetMainCamera(s_ptr<Camera> cam)
	{
		main_cam_ = cam;
	}
	void RenderManager::OnScreenSizeChange(uint32 width, uint32 height)
	{
		resolution_width_ = width;
		resolution_height_ = height;
	}
	void RenderManager::OnClearContextStates()
	{
		BindPSSamplerStates(GraphicsDevice::GetInst().GetContext().Get());
	}
	void RenderManager::CreateSamplerStates(ID3D11Device* device, ID3D11DeviceContext* context)
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
	void RenderManager::CreateDebugRenderObjects(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		debug_buffer_ = std::make_unique<StructuredBuffer>();

		StructuredBuffer::BufferFlag flag =
			StructuredBuffer::BufferFlagBitMask::kSRV
			|
			StructuredBuffer::BufferFlagBitMask::kCPUDynamic;

		bool result = debug_buffer_->Create<DebugInstanceData>(device, flag, 512);
		ASSERT(result);

#pragma region //INPUT LAYOUT DESC
		s_ptr<InputLayoutDesc> input_layout_desc = std::make_shared<InputLayoutDesc>();

		for (const auto& desc : Vertex::Debug::kInputLayoutDescs)
		{
			input_layout_desc->AddLayoutDesc(desc);
		}
#pragma endregion //INPUT LAYOUT DESC

		//Debug DSS
		s_ptr<DepthStencilState> dss = std::make_shared<DepthStencilState>();

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
		if (false == dss->Create(device, dss_desc))
		{
			ASSERT_RELEASE(false);
		}

#pragma region //MESH
		debug_rect_mesh_ = std::make_unique<Mesh>();

		//VERTEX BUFFER
		auto vb = std::make_shared<VertexBuffer>();
		std::vector<Vertex::Debug::Vertex> vertices;
		vertices.resize(4);
		vertices[0].position = { -0.5f, 0.5f, 0.0f };
		vertices[1].position = { 0.5f, 0.5f, 0.0f };
		vertices[2].position = { 0.5f, -0.5f, 0.0f };
		vertices[3].position = { -0.5f, -0.5f, 0.0f };
		vb->Create(device, vertices);

		//INDEX BUFFER
		auto ib = std::make_shared<IndexBuffer>();
		std::vector<UINT> indices;
		indices.push_back(0u);
		indices.push_back(1u);
		indices.push_back(2u);
		indices.push_back(3u);
		indices.push_back(0u);
		ib->Create(device, indices, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		debug_rect_mesh_->SetBuffers(vb, ib);
#pragma endregion //MESH

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