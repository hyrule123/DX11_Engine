#include "Engine/Core/pch.h"
#include "RendererRegistry.h"

#include <Engine/Game/Component/Renderer.h>

#include <Engine/Core/Debug.h>

#include <cfloat>

namespace engine
{
    uint32_t RendererRegistry::Allocate(Renderer* renderer)
    {
        ASSERT(renderer);

        uint32_t id = kInvalid32;

		// 빈 슬롯이 있으면 재사용
        if (free_slot_head_ != kInvalid32)
        {
            id = free_slot_head_;

            ASSERT_F(sparse_slots_[id].renderer == nullptr, "재사용되는 슬롯이 비어 있지 않습니다.");

            free_slot_head_ = sparse_slots_[id].next_free_slot;
        }
        else // 없을 시 새로 추가
        {
            id = (uint32_t)sparse_slots_.size();
            ASSERT_F(id < kMaxRendererCount, "Renderer 수가 kMaxRendererCount(sort_key 24비트) 한도를 넘었습니다.");
            sparse_slots_.push_back({});
        }

        SparseSlot& slot = sparse_slots_[id];

		slot.renderer = renderer;
		slot.next_free_slot = kInvalid32;
        slot.dense_idx = (uint32_t)dense_bounds_.size();
        //slot.is_in_dirty_list; // 이미 Release된 항목이 Dirty 상태로 변경했을 수 있음. 플래그 그대로 유지 -> nullptr 여부로 Release 되었는지 확인

        dense_to_sparse_.push_back(id);
        dense_bounds_.push_back(AABB3D::Invalid());

        // 정적 오브젝트는 Transform이 다시 Broadcast하지 않을 수 있으므로 직접 표시
        MarkBoundsDirty(id);
        return id;
    }

    void RendererRegistry::Release(uint32_t id)
    {
        ASSERT(IsValidSlot(id));

        SparseSlot& slot = sparse_slots_[id];
        ASSERT(slot.renderer);

        const uint32_t dense_idx = slot.dense_idx;
        const uint32_t last = (uint32_t)dense_bounds_.size() - 1;

        // 마지막 원소를 빈자리로 옮기고, 옮겨진 원소의 sparse 연결 갱신
        if (dense_idx != last)
        {
            const uint32_t moved_id = dense_to_sparse_[last];

            dense_bounds_[dense_idx] = dense_bounds_[last];
            dense_to_sparse_[dense_idx] = moved_id;
            sparse_slots_[moved_id].dense_idx = dense_idx;
        }
        dense_bounds_.pop_back();
        dense_to_sparse_.pop_back();

        // 슬롯 비우고 free list에 반환
        slot.renderer = nullptr;
		//slot.is_in_dirty_list; // 이 플래그는 그대로 유지, Dirty Queue에 들어 있으면 true
        slot.dense_idx = kInvalid32;
        slot.next_free_slot = free_slot_head_;
        free_slot_head_ = id;

        // 불변식 검사
#ifdef _DEBUG
        if (dense_idx < dense_to_sparse_.size())
        {
            ASSERT(sparse_slots_[dense_to_sparse_[dense_idx]].dense_idx == dense_idx);
        }
#endif
    }

    void RendererRegistry::MarkBoundsDirty(uint32_t id)
    {
        ASSERT(IsValidSlot(id));

        SparseSlot& slot = sparse_slots_[id];
        ASSERT_F(slot.renderer, "해제된 슬롯에 dirty 통지가 들어왔습니다. Transform 구독 해제를 확인하세요.");

        if (slot.is_in_dirty_list)
        {
            return;
        }

        slot.is_in_dirty_list = true;
        dirty_slot_ids_.push_back(id);
    }

    void RendererRegistry::UpdateDirtyBounds()
    {
        for (uint32_t slot_id : dirty_slot_ids_)
        {
            SparseSlot& slot = sparse_slots_[slot_id];

            // 해제됨(Release에서 nullptr로 변경됨)
            if (slot.renderer == nullptr)
            {
                slot.is_in_dirty_list = false;
                continue;
            }

            ASSERT(slot.is_in_dirty_list); // 불변식: dirty_slots이므로 -> 항상 true여야 함
            slot.is_in_dirty_list = false;
            dense_bounds_[slot.dense_idx] = slot.renderer->ComputeWorldBounds();
        }
        dirty_slot_ids_.clear();
    }
}