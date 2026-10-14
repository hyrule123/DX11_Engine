#include "Engine/Core/pch.h"
#include "RenderManager.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Game/Component/Renderer.h>
#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/GPU/Buffer/ConstantBuffer.h>
#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>
#include <Engine/Resource/GPU/State/DepthStencilState.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/Material.h>
#include <Engine/Resource/GPU/PipelineState.h>
#include <Engine/Resource/GPU/Shader/InputLayout.h>
#include <Engine/Resource/GPU/Vertex.h>

#include <Engine/Render/RenderPass/RenderPass.h>
#include <Engine/Render/RenderPass/MeshRenderPass.h>
#include <Engine/Render/RenderPass/PresentPass.h>
#include <Engine/Render/Culling.h>
#include <Engine/Render/RenderPass/DebugDrawPass.h>

#include <Engine/HLSL/Core/Register.hlsli>
#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Core/Debug.h>

#include <bitset>


namespace engine
{
	RenderManager::RenderManager()
		: render_target_pool_()
		, main_render_target_override_(nullptr)
		, cb_per_pass_camera_(nullptr)
		, main_cam_(nullptr)
		, main_view_()
		, main_view_clear_setting_()
		, render_passes_()
		, mesh_pass_mask_()
		, active_mesh_passes_()
		, render_pass_order_()
		, instance_buffer_pool_()
		, sampler_states_()
		, renderer_registry_()
		, resolution_width_()
		, resolution_height_()
		, per_frame_binding_table_(REG_B_PER_FRAME_START, REG_B_PER_FRAME_COUNT, REG_T_PER_FRAME_START, REG_T_PER_FRAME_COUNT)
		, debug_draw_pass_(nullptr)
		, is_rendering()
	{
	}

	RenderManager::~RenderManager()
	{
	}
	void RenderManager::Init()
	{
		auto* context = GraphicsDevice::GetInst().GetContext();
		
		{
			// Default Render Passes
			RegisterRenderPass(EntityManager::CreateEntity<MeshRenderPass>(render_pass::ID::ForwardOpaque, MeshRenderPass::SortPolicy::State));
			RegisterRenderPass(EntityManager::CreateEntity<MeshRenderPass>(render_pass::ID::ForwardBlend, MeshRenderPass::SortPolicy::BackToFront));

			RegisterRenderPass(EntityManager::CreateEntity<PresentPass>());

			std::vector<render_pass::ID> default_order = { render_pass::ID::ForwardOpaque, render_pass::ID::ForwardBlend };

#if ENABLE_DEBUG_DRAW
			CreateDebugDrawPass();
			default_order.push_back(render_pass::ID::DebugDraw);
#endif
			default_order.push_back(render_pass::ID::Present);
			SetRenderPassOrder(std::move(default_order));
		}

		//CONSTANT BUFFERS
		cb_per_pass_camera_ = EntityManager::CreateEntity<ConstantBuffer>();
		cb_per_pass_camera_->Create<CameraData>();

		CreateSamplerStates(context);
		BindPSSamplerStates(context);
		
		instance_buffer_pool_.Init(context);
	}
	void RenderManager::Render()
	{
		ScopedValue<bool> scoped_val(is_rendering, true);

		auto* context = GraphicsDevice::GetInst().GetContext();

		// IOTA Instance ID Buffer
		instance_buffer_pool_.BindIotaBuffer(context);

		// Per Frame
		per_frame_binding_table_.Bind(context);

		Camera* main_cam = main_cam_.get();
		if (main_cam == nullptr)
		{
			ERR_MSG("Main Camera 없음");
			return;
		}

		// 1. bounds 갱신 (Renderer당 1회)
		renderer_registry_.UpdateDirtyBounds();

		// 2. View 구성 + 컬링
		main_view_.view.Setup(*main_cam, GetMainRenderTarget(), main_view_clear_setting_);
		CullView(main_view_);

		// 3. 전개: 보이는 Renderer → 활성 메시 패스 큐
		ExpandToMeshPasses(main_view_);

		// 4. 카메라 cbuffer (View 단위)
		cb_per_pass_camera_->Upload(context, main_view_.view.BuildCameraData());
		cb_per_pass_camera_->Bind(context, kShaderStageAllGraphicsFlags, REG_B_CAMERA);

		// 5. 출력 대상 클리어 (View당 1회)
		main_view_.view.ClearTargets(context);

		// 6. 실행 순서대로
		const RenderContext pass_context = {
			.dc = context,
			.view = main_view_.view,
			.registry = renderer_registry_,
			.instance_buffers = instance_buffer_pool_ };

		for (render_pass::ID pass_id : render_pass_order_)
		{
			RenderPass* pass = render_passes_[(size_t)pass_id].get();
			ASSERT(pass);
			pass->Execute(pass_context);
		}
	}

	void RenderManager::CullView(ViewEntry& view_entry)
	{
		culling::CullRenderers2D(view_entry.view.GetViewBounds2D(), renderer_registry_.GetDenseBounds(), renderer_registry_.GetDenseToSparseTable(), view_entry.visible_ids);
	}

	void RenderManager::ExpandToMeshPasses(const ViewEntry& view_entry)
	{
		for (render_pass::ID id : active_mesh_passes_)
		{
			GetMeshRenderPass(id)->Clear();
		}

		const auto entries = renderer_registry_.SparseSlots();

		for (uint32 renderer_id : view_entry.visible_ids)
		{
			const Renderer* renderer = entries[renderer_id].renderer;
			const float depth = view_entry.view.ComputeViewDepth(renderer_registry_.GetBound(renderer_id).GetCenter());

			for (const SubMesh& submesh : renderer->GetSubMeshes())
			{
				if (submesh.material == nullptr)
				{
					continue;
				}

				const render_pass::Flags passes = submesh.material->GetSupportedPasses() & active_mesh_passes_;
				for (render_pass::ID id : passes)
				{
					GetMeshRenderPass(id)->Push(submesh.key, renderer_id, depth, (uint32)submesh.material->GetInstanceDataStride(id));
				}
			}
		}
	}

	RenderTargetGroup* RenderManager::GetMainRenderTarget() const
	{
		if (main_render_target_override_)
		{
			return main_render_target_override_.get();
		}
		return GraphicsDevice::GetInst().GetSwapChainRenderTargetGroup();
	}

	void RenderManager::FrameEnd()
	{
		render_target_pool_.FrameEnd();
	}

	void RenderManager::SetRenderPassOrder(std::vector<render_pass::ID> order)
	{
		CHECK(order.empty() == false);

		render_pass::Flags in_order = {};
		for (render_pass::ID id : order)
		{
			ASSERT_F(in_order.Test(id) == false, "실행 순서에 같은 패스가 두 번 들어 있습니다.");
			ASSERT_F(render_passes_[(size_t)id] != nullptr, "등록되지 않은 패스가 실행 순서에 들어 있습니다.");
			in_order.Set(id);
		}

		render_pass_order_ = std::move(order);
		active_mesh_passes_ = in_order & mesh_pass_mask_;
	}

	void RenderManager::RegisterRenderer(Renderer* renderer)
	{
		ASSERT_F(is_rendering == false, "Rendering 중에는 Renderer를 등록할 수 없습니다.");
		ASSERT(renderer);
		ASSERT_F(renderer->IsRegistered() == false, "이미 등록된 Renderer입니다.");

		const uint32 id = renderer_registry_.Allocate(renderer);
		renderer->SetRendererId(id);
	}

	void RenderManager::UnregisterRenderer(Renderer* renderer)
	{
		ASSERT_F(is_rendering == false, "Rendering 중에는 Renderer 등록을 해제할 수 없습니다.");
		ASSERT(renderer);
		if (renderer->IsRegistered() == false)
		{
			return;
		}

		renderer_registry_.Release(renderer->GetRendererId());
		renderer->SetRendererId(kInvalidIdx32);
	}

	void RenderManager::MarkBoundsDirty(const Renderer* renderer)
	{
		ASSERT(renderer);
		if (renderer->IsRegistered())
		{
			renderer_registry_.MarkBoundsDirty(renderer->GetRendererId());
		}
	}

	void RenderManager::OnScreenSizeChange(uint32 width, uint32 height)
	{
		resolution_width_ = width;
		resolution_height_ = height;

		render_target_pool_.OnScreenSizeChange(width, height);

		auto* ctx = GraphicsDevice::GetInst().GetContext();
		instance_buffer_pool_.BindIotaBuffer(ctx);
	}
	void RenderManager::OnClearContextStates()
	{
		BindPSSamplerStates(GraphicsDevice::GetInst().GetContext());
	}

	void RenderManager::PushDebugRect(const DebugDrawPerInstanceData& debug_data)
	{
		if (debug_draw_pass_)
		{
			debug_draw_pass_->PushDebugRect(debug_data);
		}
	}

	void RenderManager::PushDebugCircle(const DebugDrawPerInstanceData & debug_data)
	{
		if (debug_draw_pass_)
		{
			debug_draw_pass_->PushDebugCircle(debug_data);
		}
	}

	void RenderManager::CreateDebugDrawPass()
	{
		std::unique_ptr<DebugDrawPass> debug_draw_pass = EntityManager::CreateEntity<DebugDrawPass>();
		debug_draw_pass_ = debug_draw_pass.get();
		RegisterRenderPass(std::move(debug_draw_pass));
	}

	MeshRenderPass* RenderManager::GetMeshRenderPass(render_pass::ID id)
	{
		if (mesh_pass_mask_.Test(id))
		{
			return static_cast<MeshRenderPass*>(render_passes_[(size_t)id].get());
		}
		return nullptr;
	}

	void RenderManager::RegisterRenderPassImpl(std::unique_ptr<RenderPass> render_pass, bool is_mesh_renderpass)
	{
		ASSERT(render_pass != nullptr);

		const size_t idx = (size_t)render_pass->GetID();
		CHECK(idx < render_passes_.size());
		CHECK_F(render_passes_[idx] == nullptr, "RenderPass already registered at this index");

		if (is_mesh_renderpass)
		{
			// 호출부의 is_base_of 판별로 실제 타입이 MeshRenderPass 계열임이 보장됨
			mesh_pass_mask_.Set(render_pass->GetID());
		}
		else
		{
			mesh_pass_mask_.Reset(render_pass->GetID());
		}

		render_passes_[idx] = std::move(render_pass);
	}


	void RenderManager::CreateSamplerStates(ID3D11DeviceContext* context)
	{
		sampler_states_.resize(REG_S_END);

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
			ERR_MSG_HRESULT(hr);
		}
		sampler_states_[REG_S_POINT_CLAMP.Get()] = point;
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

}