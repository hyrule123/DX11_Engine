#include "Engine/Core/pch.h"
#include "MeshRenderPass.h"

#include <Engine/Render/RendererRegistry.h>
#include <Engine/Render/RenderView.h>
#include <Engine/Render/InstanceBufferPool.h>

#include <Engine/Game/Component/Renderer.h>

#include <Engine/Resource/GPU/PipelineState.h>
#include <Engine/Resource/GPU/Material.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>

#include <Engine/HLSL/RenderPass.hlsli>

#include <algorithm>
#include <bit>

namespace engine
{
	namespace
	{
		// sort_key 레이아웃 (깊이 정책): [63..32] 깊이 | [31..8] renderer_id | [7..0] submesh
		constexpr uint32 kRendererIdBits = 24;
		constexpr uint32 kDepthShift = 32;
		constexpr uint32 kRendererIdShift = kSubMeshIndexBits;
		constexpr uint64 kRendererIdMask = ((uint64)1 << kRendererIdBits) - 1;

		static_assert(kSubMeshIndexBits + kRendererIdBits == kDepthShift, "sort_key 하위 32비트 배분이 맞지 않습니다.");
		static_assert(RendererRegistry::kMaxRendererCount == (1u << kRendererIdBits), "RendererRegistry 상한과 sort_key 비트가 다릅니다.");

		// float → 대소 관계가 보존되는 uint32
		uint32 DepthToSortable(float depth)
		{
			const uint32 bits = std::bit_cast<uint32>(depth);
			return (bits & 0x80000000u) ? ~bits : (bits | 0x80000000u);
		}

		uint64 MakeDepthSortKey(uint32 depth_bits, uint32 renderer_id, uint8 submesh_idx)
		{
			return ((uint64)depth_bits << kDepthShift)
				| (((uint64)renderer_id & kRendererIdMask) << kRendererIdShift)
				| (uint64)submesh_idx;
		}
	}

	MeshRenderPass::MeshRenderPass(render_pass::ID id, SortPolicy sort_policy)
		: Super(MeshRenderPass::kClassConcreteName, id)
		, sort_policy_(sort_policy)
	{}

	MeshRenderPass::~MeshRenderPass() = default;

	void MeshRenderPass::Push(RenderKey key, uint32 renderer_id, float depth)
	{
		uint64 sort_key = 0;

		// 정책은 패스마다 고정이라 분기 예측이 항상 맞는다
		switch (sort_policy_)
		{
		case SortPolicy::State:
		{
			sort_key = key.key;
			break;
		}
		case SortPolicy::FrontToBack:
		{
			sort_key = MakeDepthSortKey(DepthToSortable(depth), renderer_id, key.GetSubMeshIndex());
			break;
		}
		case SortPolicy::BackToFront:
		{
			// 깊이 비트만 반전 (하위 동률 비트는 그대로 두어 순서 고정)
			sort_key = MakeDepthSortKey(~DepthToSortable(depth), renderer_id, key.GetSubMeshIndex());
			break;
		}
		default:
		{
			ASSERT_F(false, "알 수 없는 SortPolicy");
			break;
		}
		}

		queue_.push_back({ .sort_key = sort_key, .key = key, .renderer_id = renderer_id });
	}

	void MeshRenderPass::Execute(const RenderContext& ctx)
	{
		if (queue_.empty())
		{
			return;
		}

		Sort();
		BindTargets(ctx);
		DrawQueue(ctx);
	}

	void MeshRenderPass::BindTargets(const RenderContext& ctx)
	{
		ctx.view.BindTargets(ctx.dc);
	}

	void MeshRenderPass::Sort()
	{
		std::sort(queue_.begin(), queue_.end(), [](const QueueItem& a, const QueueItem& b)
			{
				return a.sort_key < b.sort_key;
			});
	}

	void MeshRenderPass::DrawQueue(const RenderContext& ctx)
	{
		ID3D11DeviceContext* context = ctx.dc;
		const auto reg_slots = ctx.registry.SparseSlots();
		const render_pass::ID pass_id = GetID();

		uint32 prev_material_id = kInvalidID;
		uint32 prev_mesh_id = kInvalidID;
		PipelineState* prev_pipeline_state = nullptr;

		const size_t queue_count = queue_.size();
		size_t i = 0;
		while (i < queue_count)
		{
			const QueueItem& item = queue_[i];

			// 런 렝스: 같은 RenderKey가 연속된 구간 [i, span_end)
			size_t span_end = i + 1;
			while (span_end < queue_count && queue_[span_end].key == item.key)
			{
				++span_end;
			}
			const uint32 instance_count = (uint32)(span_end - i);

			const uint32 material_id = item.key.GetMaterialID();
			const uint32 mesh_id = item.key.GetMeshID();
			const uint8 submesh_idx = item.key.GetSubMeshIndex();

			// 그룹 첫 항목의 Renderer에서 Material/Mesh 조회 (그룹당 1회)
			Renderer* first_renderer = reg_slots[item.renderer_id].renderer;
			ASSERT(first_renderer);

			Material* mtrl = first_renderer->GetMaterial(submesh_idx);
			ASSERT(mtrl);

			if (prev_material_id != material_id)
			{
				PipelineState* pipeline_state = mtrl->GetPipelineState(pass_id);
				ASSERT_F(pipeline_state, "이 패스용 PipelineState가 없는 Material이 큐에 들어왔습니다.");

				if (prev_pipeline_state != pipeline_state)
				{
					pipeline_state->Bind(context);
					prev_pipeline_state = pipeline_state;
				}
				mtrl->Bind(context);
				prev_material_id = material_id;
			}

			// 인스턴스 데이터: stride별 버퍼를 Map(DISCARD) → 그룹 전체 기록 → SRV 바인딩
			const uint32 instance_data_stride = (uint32)mtrl->GetPerObjectDataStride(pass_id);
			if (instance_data_stride > 0)
			{
				StructuredBuffer* instance_buffer = ctx.instance_buffers.Acquire(instance_data_stride, instance_count);
				ASSERT(instance_buffer);

				{
					MapScopeDynamic map_scope = instance_buffer->MapDynamic(context);
					for (size_t j = i; j < span_end; ++j)
					{
						Renderer* renderer = reg_slots[queue_[j].renderer_id].renderer;
						renderer->WritePerObjectData(map_scope.Allocate());
					}
				}

				instance_buffer->BindSRV(context, ShaderStage::Vertex | ShaderStage::Pixel, REG_T_INSTANCE_BUFFER);
			}

			Mesh* mesh = first_renderer->GetMesh();
			ASSERT(mesh);

			if (prev_mesh_id != mesh_id)
			{
				mesh->Bind(context);
				prev_mesh_id = mesh_id;
			}
			mesh->Draw(context, instance_count, submesh_idx);

			i = span_end;
		}
	}
}