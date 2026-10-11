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

		phase_ = Phase::Allocating;

		for (auto& [stride, buffer] : buffers_per_stride_)
		{
			buffer.allocated = 0;
		}
	}
	InstanceBufferPool::Allocation InstanceBufferPool::Allocate(uint32 byte_stride, uint32 elem_count)
	{
		ASSERT(phase_ == Phase::Allocating);
		ASSERT_F(byte_stride > 0, "인스턴스 데이터 stride가 0입니다.");
		ASSERT_F(elem_count > 0, "인스턴스 개수가 0입니다.");

		InstanceBuffer& buffer = buffers_per_stride_[byte_stride];

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

		Allocation alloc;
		alloc.entry = &buffer;
		alloc.start = buffer.allocated;
		alloc.count = elem_count;

		buffer.allocated += elem_count;

		return alloc;
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