#pragma once
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Debug.h>

#include <Engine/Collision/Collision.h>

#include <span>
#include <vector>

namespace engine
{
    class Renderer;

    // RenderManager가 값으로 소유하는 Renderer 슬롯 저장소.
    // - 슬롯 인덱스(idx) = bounds_id. 해제 전까지 고정되며, 재등록(Invalidate)해도 유지된다.
    // - 내부는 SoA: 컬링 루프는 bounds_만, Gather는 visible_/entries_만 읽는다.
    class RendererRegistry
    {
    public:
        // 고정 id로 인덱싱 (구멍 있음)
        struct SparseSlot
        {
            Renderer* renderer;          // nullptr = 빈 슬롯
            uint32_t    dense_idx;        // dense 배열 내 현재 위치
            uint32_t    next_free_slot;
            bool      is_in_dirty_list;
        };

        // sort_key 하위 비트(bounds_id 24비트) 제약
        static constexpr uint32_t kMaxRendererCount = 1u << 24;

        RendererRegistry() = default;

        RendererRegistry(const RendererRegistry&) = delete;
        RendererRegistry& operator=(const RendererRegistry&) = delete;

        // 새 슬롯 발급. 첫 bounds 계산을 위해 dirty로 표시된다.
        [[nodiscard]] uint32_t Allocate(Renderer* renderer);

        // 해제.
        void Release(uint32_t idx);

        void MarkBoundsDirty(uint32_t idx);
        void UpdateDirtyBounds();

		std::span<const SparseSlot> SparseSlots() const { return sparse_slots_; }
		std::span<const AABB3D> GetDenseBounds() const { return dense_bounds_; }
		const AABB3D& GetBound(uint32_t id) const { 
            ASSERT(IsValidSlot(id));
			return dense_bounds_[sparse_slots_[id].dense_idx];
        }
		std::span<const uint32_t> GetDenseToSparseTable() const { return dense_to_sparse_; }

    private:
        bool IsValidSlot(uint32_t id) const { return id < sparse_slots_.size() && sparse_slots_[id].dense_idx != kInvalid32; }

        // Sparse Slots(고정 ID)
        std::vector<SparseSlot>  sparse_slots_;
        uint32_t              free_slot_head_ = kInvalid32;
        std::vector<uint32_t> dirty_slot_ids_;      // 고정 id 저장 → swap에 영향받지 않음

        // Dense Entries
        std::vector<AABB3D> dense_bounds_;
        std::vector<uint32_t> dense_to_sparse_;    // dense 위치 → 고정 id
    };
}


