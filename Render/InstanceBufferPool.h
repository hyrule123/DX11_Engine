#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <memory>
#include <unordered_map>

namespace engine
{
	class StructuredBuffer;

	// stride별 동적 StructuredBuffer 풀.
	// - RenderManager가 값으로 소유하고, RenderPassContext로 패스에 전달한다.
	// - 같은 stride는 버퍼 하나를 공유한다. 그룹마다 Map(DISCARD) → 기록 → Draw까지 끝내고 넘어가므로
	//   앞 그룹의 기록이 다음 그룹의 DISCARD로 사라져도 무방하다.
	// - 버퍼는 회수하지 않고 영구 보존하며, 용량이 부족하면 1.5배(또는 요청량)로 확장한다.
	class InstanceBufferPool
	{
	public:
		InstanceBufferPool();
		~InstanceBufferPool();

		InstanceBufferPool(const InstanceBufferPool&) = delete;
		InstanceBufferPool& operator=(const InstanceBufferPool&) = delete;

		// elem_count개 이상을 담을 수 있는 stride용 버퍼를 반환한다.
		[[nodiscard]] StructuredBuffer* Acquire(uint32 byte_stride, uint32 elem_count);

	private:
		// 처음 만들 때 너무 작게 잡아 확장이 반복되는 것을 방지
		static constexpr uint32 kMinInitialCapacity = 64;

		std::unordered_map<uint32, std::unique_ptr<StructuredBuffer>> buffers_per_stride_ = {};
	};
}