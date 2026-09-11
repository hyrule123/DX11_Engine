#include "Engine/Core/pch.h"
#include "Culling.h"

#include <Engine/Collision/CollisionSystem2D.h>

#include <Engine/Game/Component/Renderer.h>

namespace engine::culling
{
	void CullRenderers2D(const AABB2D& view_bounds, std::span<RendererInfo2D> renderer_infos, std::vector<uint32>& out_visible)
	{
		out_visible.clear();
		out_visible.reserve(renderer_infos.size());

		for (uint32 i = 0; i < (uint32)renderer_infos.size(); ++i)
		{
			RendererInfo2D& info = renderer_infos[i];

			if (info.is_dirty)
			{
				info.world_bounds = info.renderer->ComputeWorldBounds2D();
				info.is_dirty = false;
			}

			if (view_bounds.Overlaps(info.world_bounds))
			{
				out_visible.push_back(i);
			}
		}
	}
}