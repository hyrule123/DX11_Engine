#include "Engine/Core/pch.h"
#include "ForwardOpaqueRenderPass.h"

#include <Engine/Game/Component/Renderer.h>

#include <Engine/Resource/Graphics/Buffer/StructuredBuffer.h>
#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/Graphics/Mesh.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/CppShared/Register.hlsli>

#include <algorithm>
#include <limits>

namespace engine
{
	ForwardOpaqueRenderPass::ForwardOpaqueRenderPass()
		: Super(ForwardOpaqueRenderPass::kClassConcreteName, RenderPassOrder::kForwardOpaque)
	{}
	ForwardOpaqueRenderPass::~ForwardOpaqueRenderPass()
	{}
	void ForwardOpaqueRenderPass::Execute(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		std::sort(render_queue_.begin(), render_queue_.end());

		if (!render_queue_.empty())
		{
			uint32 prev_material_ID = std::numeric_limits<uint32>::max();

			for (size_t i = 0; i < render_queue_.size(); ++i)
			{
				uint32 cur_material_ID = render_queue_[i].key.material_id;
				uint32 cur_mesh_ID = render_queue_[i].key.mesh_id;

				//이전 Material과 다를 경우 Material 미리 바인딩
				if (prev_material_ID != cur_material_ID)
				{
					Material* mtrl = render_queue_[i].renderer->GetMaterial().get();
					ASSERT(mtrl);
					mtrl->BindShaderSet(context, GetPassOrder());
					mtrl->BindTextures(context, ShaderStage::kPS);

					prev_material_ID = cur_material_ID;
				}

				//현재 Pair의 끝을 순회돌면서 탐색
				size_t span_end = i + 1;
				while (span_end < render_queue_.size())
				{
					if (cur_material_ID != render_queue_[span_end].key.material_id
						||
						cur_mesh_ID != render_queue_[span_end].key.mesh_id)
					{
						break;
					}
					++span_end;
				}

				//버퍼 사이즈 계산
				size_t instances_count = span_end - i;
				size_t instance_data_stride = render_queue_[i].renderer->GetPerInstanceDataStride();
				size_t total_instance_data_size = instance_data_stride * instances_count;

				//구조화 버퍼 탐색 및 업로드
				InstancingDataBufferCache& cache = instancing_data_buffers_[render_queue_[i].key];

				//캐시에 없을 시 새로 생성
				if (!cache.struct_buffer)
				{
					cache.struct_buffer = std::make_unique<StructuredBuffer>();

					constexpr StructuredBuffer::BufferFlag flag = StructuredBuffer::kSRV | StructuredBuffer::kCPUDynamic;

					bool result =
						cache.struct_buffer->Create(device, flag, instance_data_stride, instances_count);

					ASSERT(result);
				}

				// 사이즈 부족 시 2배 크기로 resize
				if (cache.struct_buffer->GetTotalByteSize() < total_instance_data_size)
				{
					bool result = cache.struct_buffer->Resize(device, context, instances_count * 2, false);
					ASSERT(result);
				}

				// StructuredBuffer에 업로드할 데이터 생성
				cache.data_storage.resize(total_instance_data_size);
				for (size_t j = 0; j < instances_count; ++j)
				{
					render_queue_[i + j].renderer->WritePerObjData(&(cache.data_storage[j * instance_data_stride]));
				}

				cache.struct_buffer->Upload(context, cache.data_storage.data(), total_instance_data_size);
				cache.struct_buffer->BindSRV(context, SLOT_T_PER_INSTANCE, ShaderStage::kVS | ShaderStage::kPS);

				//렌더링
				Mesh* mesh = render_queue_[i].renderer->GetMesh().get();
				ASSERT(mesh);
				mesh->Draw(context, instances_count);

				//인덱스 이동
				i = span_end - 1;
			}
		}

		render_queue_.clear();
	}
}