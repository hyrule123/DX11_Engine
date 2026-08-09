#include "Engine/Core/pch.h"
#include "HandlePointer.h"

#include <Engine/Core/Debug.h>

namespace engine
{
    HandlePointerPool::HandlePointerPool() {}
    HandlePointerPool::~HandlePointerPool() {}

    Handle HandlePointerPool::Create(void* object)
    {
        if (!object) { return Handle(); }

        uint32 slot_idx;

        // 여유 슬롯이 없으면 새 슬롯을 추가
        if (next_free_index_ == kUint32Max)
        {
            ASSERT(slots_.size() < kUint32Max);
            slot_idx = (uint32)(slots_.size());
            slots_.push_back({ object , });
        }
        // 여유 슬롯이 있으면 해당 슬롯을 사용
        else
        {
            ASSERT(slots_.size() > next_free_index_);
            slot_idx = next_free_index_;
            next_free_index_ = slots_[slot_idx].next_free;
            slots_[slot_idx].object = object;
            //Generation은 Destroy에서 증가시킴
        }

        return Handle(slot_idx, slots_[slot_idx].generation);
    }
    void HandlePointerPool::Destroy(Handle handle)
    {
        //이미 Invalidate 된 경우 무시
        if (!IsAlive(handle)) { return; }
        Slot& slot = slots_[handle.GetSlotID()];
        slot.object = nullptr;

        //Generation에 1을 더함, 만약 최대치에 도달하면 더 이상 재사용하지 않음
        if (++slot.generation < kUint32Max)
        {
            slot.next_free = next_free_index_;
            next_free_index_ = handle.GetSlotID();
        }
    }
}