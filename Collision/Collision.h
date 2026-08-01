#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>
#include <Engine/Core/Constant.h>

namespace engine
{
    // POD class
    class ColliderID
    {
    public:
        ColliderID() = delete;

        ColliderID(uint32 _1, uint32 _2) noexcept
            : id_((_1 < _2)
                ? ((static_cast<uint64>(_2) << 32) | static_cast<uint64>(_1))
                : ((static_cast<uint64>(_1) << 32) | static_cast<uint64>(_2)))
        {}

        uint64 GetID() const noexcept { return id_; }

        //ID중 낮은쪽
        uint32 GetLo() const noexcept { return static_cast<uint32>(id_); }

        //ID중 높은쪽
        uint32 GetHi() const noexcept { return static_cast<uint32>(id_ >> 32); }

        bool operator==(const ColliderID& _other) const noexcept = default;

    private:
        //낮은 ID를 하위 32비트, 높은 ID를 상위 32비트에 패킹
        uint64 id_;
    };

    struct ColliderID_Hasher
    {
        static_assert(sizeof(ColliderID) == sizeof(size_t), "사이즈 미일치. 처리함수를 만드세요.");
        size_t operator()(ColliderID _id) const noexcept {
            return static_cast<size_t>(_id.GetID());
        }
    };

	enum class ColliderShape2D
	{
		AABB,
		OBB,
		Circle,
		END
	};

    struct AABB2D
    {
        float2 left_bottom;
        float2 right_top;

        float2 GetCenter()      const noexcept { return (left_bottom + right_top) * 0.5f; }
        float2 GetHalfExtents() const noexcept { return (right_top - left_bottom) * 0.5f; }
    };
}