#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>
#include <Engine/Core/Constant.h>

#include <Engine/ThirdParty/ankerl/unordered_dense.h>

namespace engine
{
    class Collider2D;

	constexpr uint64 kDefaultCollisionBucketSize = 1ull << 14;
	constexpr uint64 kMaxCollisionBucketSize = 1ull << 20;
    constexpr int64 kMaxCellsPerCollider = 128;	//한 Collider가 차지할 수 있는 Cell의 최대 갯수

    // POD class
    class ColliderPairID
    {
    public:
        ColliderPairID() = delete;

        ColliderPairID(uint32 _1, uint32 _2) noexcept
            : id_((_1 < _2)
                ? ((static_cast<uint64>(_2) << 32) | static_cast<uint64>(_1))
                : ((static_cast<uint64>(_1) << 32) | static_cast<uint64>(_2)))
        {}

        uint64 GetID() const noexcept { return id_; }

        //ID중 낮은쪽
        uint32 GetLo() const noexcept { return static_cast<uint32>(id_); }

        //ID중 높은쪽
        uint32 GetHi() const noexcept { return static_cast<uint32>(id_ >> 32); }

        bool operator==(const ColliderPairID& _other) const noexcept = default;

    private:
        //낮은 ID를 하위 32비트, 높은 ID를 상위 32비트에 패킹
        uint64 id_;
    };

    struct ColliderPairID_Hasher
    {
		// 해시값이 제대로 섞여있을 경우에는 아래 태그를 켜 주면 값을 그대로 받아서 사용
		// 지금은 해시값이 섞여있지 않으므로 주석
		//using is_avalanching = void;

        static_assert(sizeof(ColliderPairID) == sizeof(size_t), "사이즈 미일치. 처리함수를 만드세요.");
        size_t operator()(ColliderPairID _id) const noexcept {
            return _id.GetID();
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
        float2 GetSize()        const noexcept { return (right_top - left_bottom); }
		void SetSize(float2 size)  noexcept { 
            left_bottom -= size * 0.5f; 
            right_top += size * 0.5f; 
        }
    };

    struct Collision2D
    {
		Collider2D* other_collider = {};
        float2 contact_point = {};
    };

	enum class CollisionEventType : uint32
	{
		kEnter,
		kStay,
		kExit
	};
}