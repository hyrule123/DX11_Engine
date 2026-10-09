#include "Engine/Core/pch.h"
#include "InstanceBufferPool.h"

#include <Engine/Resource/GPU/Buffer/StructuredBuffer.h>

#include <algorithm>

namespace engine
{
	InstanceBufferPool::InstanceBufferPool() = default;
	InstanceBufferPool::~InstanceBufferPool() = default;

	StructuredBuffer* InstanceBufferPool::Acquire(uint32 byte_stride, uint32 elem_count)
	{
		ASSERT_F(byte_stride > 0, "인스턴스 데이터 stride가 0입니다.");
		ASSERT_F(elem_count > 0, "인스턴스 개수가 0입니다.");

		std::unique_ptr<StructuredBuffer>& buffer = buffers_per_stride_[byte_stride];

		if (buffer == nullptr)
		{
			buffer = EntityManager::CreateEntity<StructuredBuffer>();
			const bool result = buffer->CreateDynamicBuffer(byte_stride, std::max(elem_count, kMinInitialCapacity));
			CHECK(result);
			return buffer.get();
		}

		const uint32 capacity = buffer->GetCapacity();
		if (elem_count > capacity)
		{
			// 1.5배, 그래도 부족하면 요청량만큼
			const uint32 grown = capacity + capacity / 2;
			const bool result = buffer->Reserve(std::max(grown, elem_count));
			CHECK(result);
		}

		return buffer.get();
	}
}