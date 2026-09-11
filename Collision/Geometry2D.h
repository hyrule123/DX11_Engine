#pragma once

#include <Engine/Collision/Collision.h>

namespace engine::geometry_2d
{
	inline bool Overlap(const AABB2D& a, const AABB2D& b)
	{
		const bool x_overlap = (a.min.x <= b.max.x) && (a.max.x >= b.min.x);
		const bool y_overlap = (a.min.y <= b.max.y) && (a.max.y >= b.min.y);
		return x_overlap && y_overlap;
	}

	// local bounds(AABB3D) + World Matrix -> World Bounds(AABB2D)
	AABB2D TransformBoundsTo2D(const AABB3D& local_bounds, const matrix& world_mat);
}

