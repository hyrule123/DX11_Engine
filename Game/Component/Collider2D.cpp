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
		transform_ = GetComponent<Transform>();

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

	void Collider2D::CollisionEnter2D(const Collision2D& col_info)
	{
		++contact_count_;
		GetOwnerGameObject()->BroadcastCollisionEnter2D(col_info);
	}

	void Collider2D::CollisionStay2D(const Collision2D & col_info)
	{
		GetOwnerGameObject()->BroadcastCollisionStay2D(col_info);
	}

	void Collider2D::CollisionExit2D(Collider2D * other)
	{
		--contact_count_;
		GetOwnerGameObject()->BroadcastCollisionExit2D(other);
	}

	void Collider2D::TriggerEnter2D(Collider2D * other)
	{
		++contact_count_;
		GetOwnerGameObject()->BroadcastTriggerEnter2D(other);
	}

	void Collider2D::TriggerStay2D(Collider2D * other)
	{
		GetOwnerGameObject()->BroadcastTriggerStay2D(other);
	}

	void Collider2D::TriggerExit2D(Collider2D * other)
	{
		--contact_count_;
		GetOwnerGameObject()->BroadcastTriggerExit2D(other);
	}

	void Collider2D::OnLayerChanged(uint32 prev_layer, uint32 new_layer)
	{
		Super::OnLayerChanged(prev_layer, new_layer);

		CollisionSystem2D* collision_system = GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D();

		collision_system->UnregisterCollider(this);
		collision_system->RegisterCollider(this);

		DEBUG_LOG("Collider2D::OnLayerChanged() - Collider re-registered in CollisionSystem2D due to layer change.");
	}
	const AABB2D& Collider2D::GetWorldBounds() const
	{
		uint32 ver = transform_->GetVersion();
		if (ver != tf_last_seen_version_) {
			tf_last_seen_version_ = ver;
			world_bounds_ = ComputeWorldBounds();
		}
		return world_bounds_;
	}
}