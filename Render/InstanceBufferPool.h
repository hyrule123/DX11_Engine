#pragma once
#include <Engine/Core/CoreMinimal.h>

#include <Engine/Render/RenderTypes.h>
#include <Engine/Resource/GPU/Buffer/MapScopeWriteDiscard.h>

#include <memory>
#include <unordered_map>

namespace engine
{
	class StructuredBuffer;
	class InstanceIDBuffer;

	struct InstanceBufferEntry
	{
		std::unique_ptr<StructuredBuffer> buffer = {};	// stride는 buffer->GetElementStride()
		uint32_t allocated = 0;							// 이번 패스 누적 할당 수
		MapScopeWriteDiscard map_scope = {};
	};

	// stride별 동적 StructuredBuffer 풀.
	// - RenderManager가 값으로 소유하고, RenderPassContext로 패스에 전달한다.
	// - 같은 stride는 버퍼 하나를 공유한다. 
	// 그룹마다 할당 → MapAll → 기록 → UnmapAll → 드로우까지 끝내고 넘어가므로
	//   앞 그룹의 기록이 다음 그룹의 DISCARD로 사라져도 무방하다.
	// - 버퍼는 회수하지 않고 영구 보존하며, 용량이 부족하면 1.5배(또는 요청량)로 확장한다.
	class InstanceBufferPool
	{

	public:
		InstanceBufferPool();
		~InstanceBufferPool();

		void Init(ID3D11DeviceContext* dc);

		InstanceBufferPool(const InstanceBufferPool&) = delete;
		InstanceBufferPool& operator=(const InstanceBufferPool&) = delete;

		void BeginAllocation();
		// elem_count개 이상을 담을 수 있는 stride용 버퍼를 반환한다.
		[[nodiscard]] InstanceDataAllocation Allocate(uint32_t byte_stride, uint32_t elem_count);
		[[nodiscard]] DataBlock NextSlot(const InstanceDataAllocation& alloc);
		
		void MapAll(ID3D11DeviceContext* dc);

		void UnmapAll();

		void BindIotaBuffer(ID3D11DeviceContext* dc) const;

		// Idle 단계(UnmapAll 이후)에서만. alloc.entry->buffer->BindSRV(...)
		void Bind(ID3D11DeviceContext* dc, const InstanceDataAllocation& alloc, ShaderStageFlags stages, RegisterT slot);

	private:
		enum class Phase
		{
			Idle,
			Allocating,
			Mapping,
		};
		bool TransitionPhase(Phase from, Phase to);

		// 처음 만들 때 너무 작게 잡아 확장이 반복되는 것을 방지
		static constexpr uint32_t kMinInitialCapacity = 64;


	private:
		Phase phase_;
		std::unordered_map<uint32_t, InstanceBufferEntry> buffers_per_stride_;
		std::unique_ptr<InstanceIDBuffer> instance_iota_buffer;
	};
}