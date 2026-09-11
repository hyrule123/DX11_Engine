#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>
#include <Engine/Core/Constant.h>

#include <Engine/ThirdParty/ankerl/unordered_dense.h>

namespace engine
{
    class Collider2D;

	constexpr uint64 kDefaultCollisionBucketSize = 1ull << 14;
	constexpr uint64 kMaxCollisionBucketCount = 1ull << 20;
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

    // TODO: min/max로 전환
    struct AABB2D
    {
        float2 min = kFloat2Max;
        float2 max = kFloat2Lowest;

        float2 GetCenter()      const noexcept { return (min + max) * 0.5f; }
        float2 GetHalfExtents() const noexcept { return (max - min) * 0.5f; }
        float2 GetSize()        const noexcept { return (max - min); }
		void SetSize(float2 size)  noexcept { 
            min -= size * 0.5f; 
            max += size * 0.5f; 
        }

		bool Overlaps(const AABB2D& other) const noexcept
		{
            const bool x_overlap = (min.x <= other.max.x) && (max.x >= other.min.x);
            const bool y_overlap = (min.y <= other.max.y) && (max.y >= other.min.y);
            return x_overlap && y_overlap;
		}

		void Encapsulate(const float2& point) noexcept
		{
			min = float2::Min(min, point);
			max = float2::Max(max, point);
		}
    };

    struct AABB3D
    {
        float3 min = kFloat3Max;
        float3 max = kFloat3Lowest;

		float3 GetCenter()      const noexcept { return (min + max) * 0.5f; }
		float3 GetHalfExtents() const noexcept { return (max - min) * 0.5f; }
		float3 GetSize()        const noexcept { return (max - min); }
		bool IsValid()          const noexcept { return min.x <= max.x && min.y <= max.y && min.z <= max.z; }

        // 점 하나를 포함(Bounds 계산 용)
		void Encapsulate(const float3& point) noexcept
		{
			min = float3::Min(min, point);
			max = float3::Max(max, point);
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