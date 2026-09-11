#pragma once

#include <Engine/Collision/Collision.h>	//AABB2D

namespace engine
{
	class Renderer;
	struct RendererInfo2D
	{
		Renderer* renderer;
		AABB2D world_bounds;
		bool is_dirty;
	};

	struct RenderPassContext
	{
		AABB2D view_bounds_2d;
	};
}