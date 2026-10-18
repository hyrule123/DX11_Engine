#pragma once
#include <Engine/Core/CoreMinimal.h>

#include <Engine/Render/RenderTypes.h>

#include <span>

namespace engine::culling
{
	// dense_bounds[i]가 view_bounds와 겹치면 dense_to_sparse[i](고정 id)를 out_visible_ids에 추가
	void CullRenderers2D(
		const AABB2D& view_bounds,
		std::span<const AABB3D> dense_bounds,
		std::span<const uint32_t> dense_to_sparse,
		std::vector<uint32_t>& out_visible_ids);
}