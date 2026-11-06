#include "Engine/Core/pch.h"
#include "HandlePointer.h"

#include <Engine/Core/Debug.h>

namespace engine
{
    HandlePointerPool::HandlePointerPool() {}
    HandlePointerPool::~HandlePointerPool() 
	{
		for (uint32_t i = 0; i < kMaxChunkCount; ++i)
		{
			delete[] chunks_[i].load(std::memory_order_relaxed);
		}
	}

	Handle HandlePointerPool::Create(void* object)
	{
		if (!object) { return Handle(); }

		std::lock_guard<std::mutex> lock(mutex_);

		uint32_t slot_id = kUint32Max;

		// 여유 슬롯이 없으면 새 슬롯을 추가
		if (next_free_index_ == kUint32Max)
		{
			CHECK_F(allocated_slot_count_ < kMaxChunkCount * kSlotsPerChunk, "Exceeded maximum slot count");

			slot_id = allocated_slot_count_++;

			// 청크의 첫 슬롯이면 청크 할당
			if (GetSlotIndexInChunk(slot_id) == 0)
			{
				chunks_[GetChunkIndex(slot_id)].store(new Slot[kSlotsPerChunk](), std::memory_order_release);
			}
		}
		// 여유 슬롯이 있으면 해당 슬롯을 사용
		else
		{
			CHECK_F(next_free_index_ < allocated_slot_count_, "Invalid free slot index");
			slot_id = next_free_index_;
		}

		Slot* chunk = chunks_[GetChunkIndex(slot_id)].load(std::memory_order_relaxed);
		Slot& slot = chunk[GetSlotIndexInChunk(slot_id)];

		next_free_index_ = slot.next_free;
		slot.object.store(object, std::memory_order_release);

		// Generation은 Destroy에서 증가시킴
		return Handle(slot_id, slot.generation.load(std::memory_order_relaxed));
	}

    void HandlePointerPool::Destroy(Handle handle)
    {
		std::lock_guard<std::mutex> lock(mutex_);

		// generation과 청크 포인터를 쓰는 코드는 모두 같은 mutex 안에 있음. 
		// mutex의 lock이 이전 소유자의 쓰기를 모두 보이게 하므로, 잠금 안에서는 relaxed만으로도 최신화된 값을 읽는 것이 가능. 
		// 동기화를 mutex가 맡고, atomic은 잠금 없는 Resolve와의 데이터 레이스만 막아 주는 구조.

        //이미 Invalidate 된 경우 무시
        if (!IsAlive(handle)) { return; }

		const uint32_t chunk_index = GetChunkIndex(handle.GetSlotID());
		const uint32_t slot_index = GetSlotIndexInChunk(handle.GetSlotID());

		Slot* chunk = chunks_[chunk_index].load(std::memory_order_relaxed);

		Slot& slot = chunk[slot_index];
		// Resolve는 잠금 없이 읽으므로 object를 먼저 무효화하고 generation을 증가시킨다.
		slot.object.store(nullptr, std::memory_order_release);

        //Generation에 1을 더함, 만약 최대치에 도달하면 더 이상 재사용하지 않음
		const uint32_t new_generation = slot.generation.load(std::memory_order_relaxed) + 1;
		slot.generation.store(new_generation, std::memory_order_release);
        if (new_generation < kUint32Max)
        {
            slot.next_free = next_free_index_;
            next_free_index_ = handle.GetSlotID();
        }
    }
}