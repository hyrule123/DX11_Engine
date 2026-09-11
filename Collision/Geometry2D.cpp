#include "Engine/Core/pch.h"
#include "Geometry2D.h"

#include <Engine/Core/Debug.h>

namespace engine::geometry_2d
{
	AABB2D TransformBoundsTo2D(const AABB3D& local_bounds, const matrix& world_mat)
	{
		ASSERT(local_bounds.IsValid());

		// 네 모서리 구한다.
		float3 corners[4] = {
			float3(local_bounds.min.x, local_bounds.min.y, local_bounds.min.z),
			float3(local_bounds.max.x, local_bounds.min.y, local_bounds.min.z),
			float3(local_bounds.max.x, local_bounds.max.y, local_bounds.min.z),
			float3(local_bounds.min.x, local_bounds.max.y, local_bounds.min.z)
		};

		// world 좌표로 변환하고, 2D AABB로 변환
		AABB2D world_bounds = {};
		for (uint32 i = 0; i < 4; ++i)
		{
			corners[i] = float3::Transform(corners[i], world_mat);
			world_bounds.min = float2::Min(world_bounds.min, float2(corners[i]));
			world_bounds.max = float2::Max(world_bounds.max, float2(corners[i]));
		}
		return world_bounds;
	}
}