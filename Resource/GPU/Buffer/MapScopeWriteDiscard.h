#pragma once
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Debug.h>

struct ID3D11DeviceContext;
struct ID3D11Buffer;

namespace engine
{
    class MapScopeWriteDiscard
    {
    public:
        MapScopeWriteDiscard() = default;
        MapScopeWriteDiscard(ID3D11DeviceContext* ctx, ID3D11Buffer* buf, uint32 stride, uint32 capacity);
        ~MapScopeWriteDiscard();

        MapScopeWriteDiscard(MapScopeWriteDiscard&& move) noexcept;
        MapScopeWriteDiscard& operator=(MapScopeWriteDiscard&& move) noexcept;

        MapScopeWriteDiscard(const MapScopeWriteDiscard&) = delete;
        MapScopeWriteDiscard& operator=(const MapScopeWriteDiscard&) = delete;

        bool IsValid() const { return mapped_ok_; }
        uint32 Count() const { return count_; }
        bool IsFull() const { return count_ >= capacity_; }

        // 슬롯을 하나 소비한다. 실패 시 빈 블록.
        DataBlock Allocate()
        {
            ASSERT(mapped_ok_ && count_ < capacity_);
            if (!mapped_ok_ || count_ >= capacity_) { return {}; }
            return { p_data_ + (size_t)stride_ * count_++, stride_ };
        }

    private:
        void Release() noexcept;

        ID3D11DeviceContext* context_ = nullptr;
        ID3D11Buffer* buffer_ = nullptr;
        uint32 stride_ = 0;
        uint32 capacity_ = 0;

        uint8* p_data_ = nullptr;
        uint32 count_ = 0;
        bool mapped_ok_ = false;
    };
}
