#include "Engine/Core/pch.h"
#include "Culling.h"

#include <Engine/Collision/CollisionSystem2D.h>

#include <Engine/Game/Component/Renderer.h>

namespace engine::culling
{
	void CullRenderers2D(
		const AABB2D& view_bounds,
		std::span<const AABB3D> dense_bounds,
		std::span<const uint32_t> dense_to_sparse,
		std::vector<uint32_t>& out_visible_ids)
	{
		ASSERT(dense_bounds.size() == dense_to_sparse.size());

		out_visible_ids.clear();

		const size_t count = dense_bounds.size();
		for (size_t i = 0; i < count; ++i)
		{
			const AABB3D& b = dense_bounds[i];

			// 경계 포함(<=) 비교: 두께 0인 스프라이트도 빠지지 않음
			// 무효 AABB(Mesh 없음 등)는 min > max라 자연스럽게 탈락
			if (b.min.x <= view_bounds.max.x && b.max.x >= view_bounds.min.x &&
				b.min.y <= view_bounds.max.y && b.max.y >= view_bounds.min.y)
			{
				out_visible_ids.push_back(dense_to_sparse[i]);
			}
		}
	}
}