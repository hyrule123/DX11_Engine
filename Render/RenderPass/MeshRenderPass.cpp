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

	void MeshRenderPass::Push(RenderKey key, uint32 renderer_id, float depth, uint32 obj_data_stride)
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

		queue_.push_back({ .sort_key = sort_key, .key = key, .renderer_id = renderer_id, .obj_data_stride = obj_data_stride });
	}

	void MeshRenderPass::Execute(const RenderContext& ctx)
	{
		if (queue_.empty())
		{
			return;
		}

		Sort();
		BuildGroups(ctx);		// 런 렝스 + 인스턴스 범위 할당
		UploadInstances(ctx);	// stride별 Map 1회 → 일괄 기록 → Unmap
		BindTargets(ctx);
		DrawGroups(ctx);		// 상태 바인딩(변경 시) + Draw
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

	void MeshRenderPass::BuildGroups(const RenderContext& ctx)
	{
		draw_groups_.clear();
		ctx.instance_buffers.BeginAllocation();

		const auto renderer_registry = ctx.registry.SparseSlots();

		const size_t queue_count = queue_.size();
		size_t i = 0;
		while (i < queue_count)
		{
			const QueueItem& item = queue_[i];

			// 런 렝스: 같은 RenderKey가 연속된 구간 [i, span_end)
			size_t span_end = i + 1;
			while (span_end < queue_count && queue_[span_end].key == item.key)
			{
				ASSERT(queue_[span_end].obj_data_stride == item.obj_data_stride);
				++span_end;
			}
			const uint32 instance_count = (uint32)(span_end - i);

			DrawGroup& group = draw_groups_.emplace_back();
			group.queue_begin = (uint32)i;
			group.instance_count = instance_count;
			group.alloc = (item.obj_data_stride > 0)
				? ctx.instance_buffers.Allocate(item.obj_data_stride, instance_count)
				: InstanceDataAllocation{};
			group.renderer = renderer_registry[item.renderer_id].renderer;
			ASSERT(group.renderer);

			i = span_end;
		}
	}

	void MeshRenderPass::UploadInstances(const RenderContext& ctx)
	{
		InstanceBufferPool& pool = ctx.instance_buffers;
		const auto renderer_registry = ctx.registry.SparseSlots();

		pool.MapAll(ctx.dc);

		// 그룹 순서 = 할당 순서이므로 stride별 커서가 alloc.start와 일치한다
		for (const DrawGroup& group : draw_groups_)
		{
			if (false == group.alloc.IsValid())
			{
				continue;	// per-object 데이터 없음: Renderer 역참조도 생략
			}

			ASSERT(group.queue_begin + group.instance_count <= (uint32)queue_.size());

			const uint32 queue_end = group.queue_begin + group.instance_count;
			for (uint32 q = group.queue_begin; q < queue_end; ++q)
			{
				Renderer* renderer = renderer_registry[queue_[q].renderer_id].renderer;
				ASSERT(renderer);
				renderer->WriteInstanceData(pool.NextSlot(group.alloc));
			}
		}

		// 중간 return 금지: Mapping 단계에 남으면 다음 패스의 BeginAllocation에서 걸린다
		pool.UnmapAll();
	}

	void MeshRenderPass::DrawGroups(const RenderContext& ctx)
	{
		ID3D11DeviceContext* context = ctx.dc;
		InstanceBufferPool& pool = ctx.instance_buffers;
		const render_pass::ID pass_id = GetID();

		uint32 prev_material_id = kInvalidID;
		uint32 prev_mesh_id = kInvalidID;
		PipelineState* prev_pipeline_state = nullptr;
		const void* prev_instance_buffer = nullptr;		// SRV 재바인딩 판단용 (alloc.entry 비교)

		for (const DrawGroup& group : draw_groups_)
		{
			const RenderKey key = queue_[group.queue_begin].key;
			const uint32 material_id = key.GetMaterialID();
			const uint32 mesh_id = key.GetMeshID();
			const uint8 submesh_idx = key.GetSubMeshIndex();

			// Material / PipelineState (Material이 바뀔 때만 조회)
			if (prev_material_id != material_id)
			{
				Material* mtrl = group.renderer->GetMaterial(submesh_idx);
				ASSERT(mtrl);

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

			// 인스턴스 데이터 SRV (stride 버퍼가 바뀔 때만)
			if (group.alloc.IsValid() && prev_instance_buffer != group.alloc.entry)
			{
				pool.Bind(context, group.alloc, ShaderStage::Vertex | ShaderStage::Pixel, REG_T_INSTANCE_BUFFER);
				prev_instance_buffer = group.alloc.entry;
			}

			// Mesh (바뀔 때만)
			Mesh* mesh = group.renderer->GetMesh();
			ASSERT(mesh);
			if (prev_mesh_id != mesh_id)
			{
				mesh->Bind(context);
				prev_mesh_id = mesh_id;
			}

			mesh->Draw(context,
				{
					.instance_start = group.alloc.IsValid() ? group.alloc.start : 0,
					.instance_count = group.instance_count,
					.submesh_index = submesh_idx,
				});
		}
	}
}