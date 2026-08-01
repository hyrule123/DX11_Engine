#include "Engine/Core/pch.h"
#include "Collider2D.h"

#include <Engine/Core/Debug.h>

#include <Engine/Game/Component/Transform.h>
#include <Engine/Game/Scene.h>

#include <Engine/Collision/CollisionSystem2D.h>

namespace engine
{
	Collider2D::Collider2D(const HashedStringView& concrete_class_name, ColliderShape2D shape)
		: Component(concrete_class_name, Collider2D::kComponentCategory)
		, shape_(shape)
	{}
	Collider2D::~Collider2D()
	{}
	void Collider2D::Awake()
	{
		Super::Awake();
		transform_ = GetComponent<Transform>().get();

		ASSERT(transform_);
	}
	void Collider2D::OnEnable()
	{
		Super::OnEnable();
		tf_last_seen_version_ = transform_->GetVersion();
		world_bounds_ = ComputeWorldBounds();

		GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D()->RegisterCollider(this);

		DEBUG_LOG("Collider2D::OnEnable() - Collider registered in CollisionSystem2D.");
	}
	void Collider2D::FixedUpdate()
	{
		Super::FixedUpdate();

		if (transform_->GetVersion() != tf_last_seen_version_) {
			tf_last_seen_version_ = transform_->GetVersion();
			world_bounds_ = ComputeWorldBounds();
		}
	}
	void Collider2D::OnLayerChanged(uint32 prev_layer, uint32 new_layer)
	{
		Super::OnLayerChanged(prev_layer, new_layer);

		CollisionSystem2D* collision_system = GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D();

		collision_system->UnregisterCollider(this);
		collision_system->RegisterCollider(this);

		DEBUG_LOG("Collider2D::OnLayerChanged() - Collider re-registered in CollisionSystem2D due to layer change.");
	}
}