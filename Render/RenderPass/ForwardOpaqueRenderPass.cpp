#include "Engine/Core/pch.h"
#include "ForwardOpaqueRenderPass.h"

#include <Engine/Manager/RenderManager.h>

#include <Engine/Game/Component/Renderer.h>

#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>
#include <Engine/Resource/GPU/Material.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/PipelineState.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/ForwardOpaquePass.hlsli>

#include <Engine/Collision/Geometry2D.h>

#include <Engine/Render/Culling.h>

#include <Engine/Util/IDAllocator.h>

#include <algorithm>
#include <limits>

namespace engine
{
	ForwardOpaqueRenderPass::ForwardOpaqueRenderPass()
		: Super(RenderPassOrder::ForwardOpaque)
	{}
	ForwardOpaqueRenderPass::~ForwardOpaqueRenderPass()
	{}
	void ForwardOpaqueRenderPass::Execute(ID3D11DeviceContext* context, const RenderPassContext& pass_context)
	{
		culled_renderers_.clear();
		render_queue_.clear();
		
		// 컬링 시행
		std::span<RendererInfo2D> registered_renderers = GetRegisteredRenderers();
		culling::CullRenderers2D(pass_context.view_bounds_2d, registered_renderers, culled_renderers_);

		// 컬링된 렌더러들에 대해 전개하고 등록한다.
		for (uint32 slot : culled_renderers_)
		{
			const std::vector<SubMeshRenderData>& all_submeshes_data = registered_renderers[slot].renderer->GetAllSubMeshRenderData();

			for (const auto& submesh_data : all_submeshes_data)
			{
				if (submesh_data.pass_flags.Test(GetPassOrder()))
				{
					render_queue_.push_back({ .key = submesh_data.key, .renderer = registered_renderers[slot].renderer });
				}
			}
		}

		// RenderKey 기준으로 정렬
		std::sort(render_queue_.begin(), render_queue_.end());

		BindRenderTargetGroup(context);

		uint32 prev_material_id = kInvalidID;
		uint32 prev_mesh_id = kInvalidID;
		PipelineState* prev_pipeline_state = nullptr;

		uint32 i = 0;
		while (i < render_queue_.size())
		{
			const RenderItem& item = render_queue_[i];

			const uint32 material_id = item.key.GetMaterialID();
			const uint32 mesh_id = item.key.GetMeshID();
			const uint8 submesh_idx = item.key.GetSubMeshIndex();

			uint32 span_end = i + 1;
			while (span_end < render_queue_.size() && item.key == render_queue_[span_end].key)
			{
				++span_end;
			}

			const uint32 instances_count = (uint32)(span_end - i);

			Material* mtrl = item.renderer->GetMaterial(submesh_idx);
			ASSERT(mtrl);

			if (prev_material_id != material_id)
			{
				PipelineState* pipeline_state = mtrl->GetPipelineState(GetPassOrder());
				if (prev_pipeline_state != pipeline_state)
				{
					pipeline_state->Bind(context);
					prev_pipeline_state = pipeline_state;
				}
				mtrl->BindTextures(context, ShaderStage::Pixel);
				prev_material_id = material_id;
			}

			const uint32 instance_data_stride = (uint32)mtrl->GetPerObjectDataStride(GetPassOrder());

			if (instance_data_stride > 0)
			{
				StructuredBuffer* instance_buffer = RenderManager::GetInst().AcquireInstanceBuffer(instance_data_stride, instances_count);

				ASSERT(instance_buffer);

				{
					MapScopeDynamic map_scope = instance_buffer->MapDynamic(context);
					for (size_t j = i; j < span_end; ++j)
					{
						render_queue_[j].renderer->WritePerObjectData(map_scope.Allocate());
					}
				}

				instance_buffer->BindSRV(context, ShaderStage::Vertex | ShaderStage::Pixel, REG_T_INSTANCE_BUFFER);
			}

			Mesh* mesh = item.renderer->GetMesh();
			ASSERT(mesh);

			if (prev_mesh_id != mesh_id)
			{
				mesh->Bind(context);
				prev_mesh_id = mesh_id;
			}
			mesh->Draw(context, instances_count, submesh_idx);

			i = span_end;
		}
	}
}