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

        uint32_t id = kInvalidIdx32;

		// 빈 슬롯이 있으면 재사용
        if (free_slot_head_ != kInvalidIdx32)
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
		slot.next_free_slot = kInvalidIdx32;
        slot.dense_idx = (uint32_t)dense_bounds_.size();
        slot.is_bounds_dirty = false;

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
        slot.is_bounds_dirty = false;
        slot.dense_idx = kInvalidIdx32;
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

        if (slot.is_bounds_dirty)
        {
            return;
        }

        slot.is_bounds_dirty = true;
        dirty_slot_ids_.push_back(id);
    }

    void RendererRegistry::UpdateDirtyBounds()
    {
        for (uint32_t slot_id : dirty_slot_ids_)
        {
            SparseSlot& slot = sparse_slots_[slot_id];

            // 해제됨(Release에서 false로 변경) 또는 이미 처리된 중복 항목
            if (slot.is_bounds_dirty == false)
            {
                ASSERT(slot.renderer == nullptr); // 불변식 검사
                continue;   
            }

            slot.is_bounds_dirty = false;
            dense_bounds_[slot.dense_idx] = slot.renderer->ComputeWorldBounds();
        }
        dirty_slot_ids_.clear();
    }
}