#include "Engine/Core/pch.h"
#include "InstanceBufferPool.h"

#include <Engine/Manager/RenderManager.h>

#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>
#include <Engine/Render/InstanceIDBuffer.h>	

#include <algorithm>

namespace engine
{
	InstanceBufferPool::InstanceBufferPool()
		: phase_(Phase::Idle)
		, buffers_per_stride_()
		, instance_iota_buffer(std::make_unique<InstanceIDBuffer>())
	{
	}
	InstanceBufferPool::~InstanceBufferPool() = default;

	void InstanceBufferPool::Init(ID3D11DeviceContext* dc)
	{
		instance_iota_buffer->Init(dc);
	}

	void InstanceBufferPool::BeginAllocation()
	{
		bool result = TransitionPhase(Phase::Idle, Phase::Allocating);
		if (false == result) { return; }

		for (auto& [stride, buffer] : buffers_per_stride_)
		{
			buffer.allocated = 0;
		}
	}
	InstanceDataAllocation InstanceBufferPool::Allocate(uint32 byte_stride, uint32 elem_count)
	{
		ASSERT(phase_ == Phase::Allocating);
		ASSERT_F(byte_stride > 0, "인스턴스 데이터 stride가 0입니다.");
		ASSERT_F(elem_count > 0, "인스턴스 개수가 0입니다.");

		InstanceBufferEntry& buffer = buffers_per_stride_[byte_stride];

		if (buffer.buffer == nullptr)
		{
			// 동적 할당 / 기본 생성만 진행
			buffer.buffer = EntityManager::CreateEntity<StructuredBuffer>();
			const bool result = buffer.buffer->CreateDynamicBuffer(byte_stride, std::max(elem_count, kMinInitialCapacity));
			CHECK(result);
		}

		//const uint32 required = buffer.allocated + elem_count;
		//const uint32 capacity = buffer.buffer->GetCapacity();
		//if (required > capacity)
		//{
		//	// 1.5배, 그래도 부족하면 요청량만큼
		//	const uint32 grown = capacity + capacity / 2;
		//	const bool result = buffer.buffer->Reserve(std::max(grown, elem_count));
		//	CHECK(result);
		//}

		InstanceDataAllocation alloc;
		alloc.entry = &buffer;
		alloc.start = buffer.allocated;
		alloc.count = elem_count;

		buffer.allocated += elem_count;

		return alloc;
	}

	DataBlock InstanceBufferPool::NextSlot(const InstanceDataAllocation& alloc)
	{
		ASSERT_F(phase_ == Phase::Mapping, "NextSlot은 MapAll ~ UnmapAll 사이에서만 호출할 수 있습니다.");
		ASSERT_F(alloc.IsValid(), "무효 Allocation에 기록하려 했습니다.");
		if (phase_ != Phase::Mapping || false == alloc.IsValid())
		{
			return {};	// 릴리스 방어
		}
		return alloc.entry->map_scope.Allocate();
	}

	void InstanceBufferPool::MapAll(ID3D11DeviceContext* dc)
	{
		bool result = TransitionPhase(Phase::Allocating, Phase::Mapping);
		if (false == result) { return; }
	
		uint32 alloc_count_max = 0;
		for (auto& [stride, buffer] : buffers_per_stride_)
		{
			if (buffer.allocated == 0)
			{
				continue;	// 이번 패스에 사용되지 않은 버퍼는 맵핑하지 않음
			}
			
			const uint32 capacity = buffer.buffer->GetCapacity();
			if (capacity < buffer.allocated)
			{
				// 1.5배, 그래도 부족하면 필요량만큼
				const uint32 grown = std::max(buffer.allocated, capacity + capacity / 2);
				result = buffer.buffer->Reserve(grown);
				CHECK_F(result, "InstanceBufferPool: stride {} 버퍼 예약 실패", stride);

				alloc_count_max = std::max(alloc_count_max, grown);
			}

			alloc_count_max = std::max(alloc_count_max, buffer.buffer->GetCapacity());
			buffer.map_scope = buffer.buffer->MapWriteDiscard(dc);
			ASSERT_F(buffer.map_scope.IsValid(), "InstanceBufferPool: stride {} 버퍼 맵핑 실패", stride);
		}

		result = instance_iota_buffer->EnsureCapacity(dc, alloc_count_max);
		CHECK_F(result, "InstanceBufferPool: 인스턴스 ID 버퍼 용량 확보 실패");
	}

	void InstanceBufferPool::UnmapAll()
	{
		bool result = TransitionPhase(Phase::Mapping, Phase::Idle);
		if (result == false) { return; }

		for (auto& [stride, buffer] : buffers_per_stride_)
		{
			if (buffer.map_scope.IsValid())
			{
				buffer.map_scope = {};
			}
		}
	}

	void InstanceBufferPool::BindIotaBuffer(ID3D11DeviceContext* dc) const
	{
		ASSERT(instance_iota_buffer != nullptr);
		instance_iota_buffer->Bind(dc);
	}

	void InstanceBufferPool::Bind(ID3D11DeviceContext* dc, const InstanceDataAllocation& alloc, ShaderStageFlags stages, RegisterT slot)
	{
		ASSERT(phase_ == Phase::Idle);
		if (phase_ != Phase::Idle)
		{
			return;	// 릴리스 방어
		}

		if (false == alloc.IsValid())
		{
			return;	// stride 0: 바인딩 없음
		}

		StructuredBuffer* buffer = alloc.entry->buffer.get();
		ASSERT(buffer != nullptr);

		buffer->BindSRV(dc, stages, slot);
	}

	bool InstanceBufferPool::TransitionPhase(Phase from, Phase to)
	{
		ASSERT_F(phase_ == from, "InstanceBufferPool 단계 순서 위반");
		if (phase_ != from)
		{
			return false;	// 릴리스 방어
		}
		phase_ = to;
		return true;
	}
}