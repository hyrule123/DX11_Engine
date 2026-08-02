#include "Engine/Core/pch.h"
#include "CircleCollider2D.h"

#include <Engine/Game/Component/Transform.h>

#include <Engine/Manager/RenderManager.h>

namespace engine
{
	CircleCollider2D::CircleCollider2D()
		: Collider2D(CircleCollider2D::kClassConcreteName, ColliderShape2D::Circle)
	{}
	CircleCollider2D::~CircleCollider2D()
	{}
	void CircleCollider2D::LateUpdate()
	{
		Super::LateUpdate();

#ifndef NDEBUG
		DebugInstanceData debug_data;

		debug_data.world_mat = DirectX::XMMatrixAffineTransformation(
			float3(radius_ * 2.0f, radius_ * 2.0f, 1.0f),
			DirectX::g_XMZero, // <-- float3(0,0,0) 대신 DirectX 내장 Zero 상수 사용 ( XMLoad 과정 생략 )
			DirectX::g_XMIdentityR3,
			transform_->GetWorldPosition()
		);

		debug_data.color = IsContacting() ? float3(1.0f, 0.0f, 0.0f) : float3(0.0f, 1.0f, 0.0f);
		
		RenderManager::GetInst().DrawDebugCircle(debug_data);
#endif
	}
	AABB2D CircleCollider2D::ComputeWorldBounds() const
	{
		AABB2D ret;

		float3 pos = transform_->GetWorldPosition();

		ret.left_bottom = float2(pos.x, pos.y) - float2(radius_, radius_);
		ret.right_top = float2(pos.x, pos.y) + float2(radius_, radius_);

		return ret;
	}
	float2 CircleCollider2D::GetCenter() const
	{
		float3 pos = transform_->GetWorldPosition();
		return float2(pos.x, pos.y);
	}
}