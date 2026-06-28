#include "Engine/Core/pch.h"
#include "RenderManager.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Game/Component/Renderer.h>
#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/Graphics/Buffer/ConstantBuffer.h>
#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/Graphics/Mesh.h>

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

		//CONSTANT BUFFERS
		cb_per_obj_ = std::make_shared<ConstantBuffer>();
		cb_per_obj_->Create<PerObj>(device.Get());

		cb_per_pass_ = std::make_shared<ConstantBuffer>();
		cb_per_pass_->Create<PerPass>(device.Get());

		CreateSamplerStates();
		BindPSSamplerStates();
	}
	void RenderManager::Render()
	{
		if (main_cam_.expired()) 
		{ 
			ERROR_MESSAGE("Main Camera 없음");
			return; 
		}

		auto context = GraphicsDevice::GetInst().GetContext();

		auto cam = main_cam_.lock();

		//Per Pass ( = Camera )
		PerPass per_pass_data = {};
		per_pass_data.view_mat = cam->GetViewMatrix();
		per_pass_data.proj_mat = cam->GetProjMatrix();

		cb_per_pass_->Upload(context.Get(), per_pass_data);
		cb_per_pass_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

		//오브젝트 별 렌더링
		for (const auto& renderer : render_queue_)
		{
			//Per Obj
			PerObj obj_data = renderer->GetPerObjData();

			cb_per_obj_->Upload(context.Get(), obj_data);
			cb_per_obj_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_OBJECT);

			renderer->GetMaterial()->BindAll(context.Get(), ShaderStage::kPS);
			renderer->GetMesh()->Render(context.Get());
		}
	}
	void RenderManager::FrameEnd()
	{
		render_queue_.clear();
	}
	void RenderManager::OnResolutionChange(uint32 width, uint32 height)
	{
		s_ptr<Camera> main_cam = main_cam_.lock();
		if (main_cam)
		{
			Camera::ProjectionMatrixDesc desc = main_cam->GetProjectionMatrixDesc();
			desc.width = (float)width;
			desc.height = (float)height;
			main_cam->CreateProjMatrix(desc);
		}
	}
	void RenderManager::OnClearContextStates()
	{
		BindPSSamplerStates();
	}
	void RenderManager::CreateSamplerStates()
	{
		sampler_states_.resize(SLOT_S_END);

		auto device = GraphicsDevice::GetInst().GetDevice();

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
	void RenderManager::BindPSSamplerStates()
	{
		auto context = GraphicsDevice::GetInst().GetContext();

		std::vector<ID3D11SamplerState*> raw_ptrs;
		for (size_t i = 0; i < sampler_states_.size(); ++i)
		{
			raw_ptrs.push_back(sampler_states_[i].Get());
		}

		context->PSSetSamplers(0u, (UINT)raw_ptrs.size(), raw_ptrs.data());
	}
}