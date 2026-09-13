#include "Engine/Core/pch.h"
#include "ForwardOpaqueRenderPass.h"

#include <Engine/Manager/RenderManager.h>

#include <Engine/Game/Component/Renderer.h>

#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>
#include <Engine/Resource/GPU/Material.h>
#include <Engine/Resource/GPU/Mesh.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/ForwardOpaquePass.hlsli>

#include <Engine/Collision/Geometry2D.h>

#include <Engine/Render/Culling.h>

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

			for (size_t i = 0; i < all_submeshes_data.size(); ++i)
			{
				const SubMeshRenderData& submesh_data = all_submeshes_data[i];
				if (submesh_data.pass_flags.Has(GetPassOrder()))
				{
					render_queue_.push_back({ .key = submesh_data.key, .renderer = registered_renderers[slot].renderer, .submesh_idx = (uint8)i });
				}
			}
		}

		// RenderKey 기준으로 정렬
		std::sort(render_queue_.begin(), render_queue_.end());

		BindRenderTargetGroup(context);

		if (!render_queue_.empty())
		{
			uint32 prev_material_ID = std::numeric_limits<uint32>::max();

			for (size_t i = 0; i < render_queue_.size(); ++i)
			{
				uint32 cur_material_ID = render_queue_[i].key.GetMaterialID();
				uint32 cur_mesh_ID = render_queue_[i].key.GetMeshID();

				//이전 Material과 다를 경우 Material 미리 바인딩
				if (prev_material_ID != cur_material_ID)
				{
					Material* mtrl = render_queue_[i].renderer->GetMaterial(0).get();
					ASSERT(mtrl);
					mtrl->BindPipelineState(context, GetPassOrder());
					mtrl->BindTextures(context, ShaderStage::Flags::Pixel);

					prev_material_ID = cur_material_ID;
				}

				//현재 Pair의 끝을 순회돌면서 탐색
				size_t span_end = i + 1;
				while (span_end < render_queue_.size())
				{
					if (cur_material_ID != render_queue_[span_end].key.GetMaterialID()
						||
						cur_mesh_ID != render_queue_[span_end].key.GetMeshID())
					{
						break;
					}
					++span_end;
				}

				//버퍼 사이즈 계산
				const uint32 instance_data_stride = (uint32)render_queue_[i].renderer->GetMaterial(0)->GetPerObjectDataStride(GetPassOrder());
				const uint32 instances_count = (uint32)(span_end - i);

				// Per Instance Data가 0이 아닐 경우 StructuredBuffer를 탐색 및 업로드
				if (instance_data_stride > 0)
				{
					//구조화 버퍼 탐색 및 업로드
					u_ptr<StructuredBuffer>& struct_buffer = instancing_data_buffers_[render_queue_[i].key];

					//캐시에 없을 시 새로 생성
					if (!struct_buffer)
					{
						struct_buffer = std::make_unique<StructuredBuffer>();

						bool result = struct_buffer->CreateDynamicBuffer(instance_data_stride, instances_count);
						ASSERT(result);
					}

					// 사이즈 부족 시 2배 크기로 resize
					if (instances_count > struct_buffer->GetCapacity())
					{
						bool result = struct_buffer->Reserve(instances_count * 2);
						ASSERT(result);
					}

					{
						ASSERT(struct_buffer->GetElementStride() == instance_data_stride);

						MapScopeDynamic map_scope = struct_buffer->MapDynamic(context);


						for (size_t j = 0; j < instances_count; ++j)
						{
							render_queue_[i + j].renderer->WritePerObjectData(map_scope.Allocate());
						}
					}

					struct_buffer->BindSRV(context, ShaderStage::Flags::Vertex | ShaderStage::Flags::Pixel, REG_T_INSTANCE_BUFFER);
				}

				//렌더링
				Mesh* mesh = render_queue_[i].renderer->GetMesh().get();
				ASSERT(mesh);
				mesh->Bind(context);
				mesh->Draw(context, instances_count);

				//인덱스 이동
				i = span_end - 1;
			}
		}

		render_queue_.clear();
	}
}