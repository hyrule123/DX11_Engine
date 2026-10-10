#include "Engine/Core/pch.h"
#include "AABBCollider2D.h"

#include <Engine/Game/Component/Transform.h>

#include <Engine/Manager/RenderManager.h>

namespace engine
{
	AABBCollider2D::AABBCollider2D()
		: Collider2D(AABBCollider2D::kClassConcreteName, ColliderShape2D::AABB)
	{}
	AABBCollider2D::~AABBCollider2D()
	{}
	void AABBCollider2D::LateUpdate()
	{
		Super::LateUpdate();

#if ENABLE_DEBUG_DRAW
		DebugDrawPerInstanceData debug_data;

		float2 size = local_bounds_.GetSize();
		debug_data.world_mat = DirectX::XMMatrixAffineTransformation(
			float3(size.x, size.y, 1.0f),
			DirectX::g_XMZero, // <-- float3(0,0,0) 대신 DirectX 내장 Zero 상수 사용 ( XMLoad 과정 생략 )
			DirectX::g_XMIdentityR3,
			transform_->GetWorldPosition()
		);

		debug_data.color = IsContacting() ? float3(1.0f, 0.0f, 0.0f) : float3(0.0f, 1.0f, 0.0f);

		RenderManager::GetInst().PushDebugRect(debug_data);
#endif //ENABLE_DEBUG_DRAW
	}
	AABB2D AABBCollider2D::GetWorldBounds() const
	{
		AABB2D ret;

		float3 pos = transform_->GetWorldPosition();

		ret.min = local_bounds_.min + float2(pos.x, pos.y);
		ret.max = local_bounds_.max + float2(pos.x, pos.y);

		return ret;
	}
}