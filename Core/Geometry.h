#pragma once

#include <Engine/Core/Math.h>
#include <Engine/Core/Constants.h>

namespace engine
{
	struct AABB2D
	{
		float2 min = kFloat2Max;
		float2 max = kFloat2Lowest;

		float2 GetCenter()      const noexcept { return (min + max) * 0.5f; }
		float2 GetHalfExtents() const noexcept { return (max - min) * 0.5f; }
		float2 GetSize()        const noexcept { return (max - min); }

		bool IsValid()          const noexcept { return min.x <= max.x && min.y <= max.y; }

		// Size > 0
		void SetSize(float2 size)  noexcept {
			min = -(size * 0.5f);
			max = size * 0.5f;
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

		constexpr static AABB3D Invalid() noexcept
		{
			AABB3D bounds;
			bounds.min = kFloat3Max;
			bounds.max = kFloat3Lowest;
			return bounds;
		}

		bool operator==(const AABB3D& other) const noexcept
		{
			return min == other.min && max == other.max;
		}
	};

	inline bool Overlap2D(const AABB2D& a, const AABB2D& b)
	{
		const bool x_overlap = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);
		const bool y_overlap = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);
		return x_overlap && y_overlap;
	}

	// local bounds(AABB3D) + World Matrix -> World Bounds(AABB2D)
	AABB2D TransformBoundsTo2D(const AABB3D& local_bounds, const matrix& world_mat);
}

