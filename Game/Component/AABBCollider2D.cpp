#include "Engine/Core/pch.h"
#include "AABBCollider2D.h"

#include <Engine/Game/Component/Transform.h>

namespace engine
{
	AABBCollider2D::AABBCollider2D()
		: Collider2D(AABBCollider2D::kClassConcreteName, ColliderShape2D::AABB)
	{}
	AABBCollider2D::~AABBCollider2D()
	{}
	AABB2D AABBCollider2D::ComputeWorldBounds() const
	{
		AABB2D ret = {};

		float3 pos = transform_->GetWorldPosition();

		ret.left_bottom = local_bounds_.left_bottom + float2(pos.x, pos.y);
		ret.right_top = local_bounds_.right_top + float2(pos.x, pos.y);

		return ret;
	}
}