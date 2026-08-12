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
	void Collider2D::Init()
	{
		Super::Init();

		Subscribe(SubscribeType::kTransformDirty);
		Subscribe(SubscribeType::kLayerChanged);
	}
	void Collider2D::Awake()
	{
		Super::Awake();
		transform_ = GetComponent<Transform>();
		ASSERT(transform_);

		collision_system_ = GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D();
	}
	void Collider2D::OnEnable()
	{
		Super::OnEnable();
		GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D()->RegisterCollider(this);

		DEBUG_LOG("Collider2D::OnEnable() - Collider registered in CollisionSystem2D.");
	}

	void Collider2D::OnDisable()
	{
		Super::OnDisable();

		GetOwnerGameObject()->GetOwnerScene()->GetCollisionSystem2D()->UnregisterCollider(this);
	}

	void Collider2D::OnTransformDirty(Transform* transform)
	{
		Super::OnTransformDirty(transform);
		
		collision_system_->SetWorldBoundsDirty(this);
	}

	void Collider2D::CollisionEnter2D(const Collision2D& col_info)
	{
		GetOwnerGameObject()->BroadcastCollisionEnter2D(col_info);
	}

	void Collider2D::CollisionExit2D(Collider2D* other)
	{
		GetOwnerGameObject()->BroadcastCollisionExit2D(other);
	}

	void Collider2D::TriggerEnter2D(Collider2D* other)
	{
		GetOwnerGameObject()->BroadcastTriggerEnter2D(other);
	}

	void Collider2D::TriggerExit2D(Collider2D* other)
	{
		GetOwnerGameObject()->BroadcastTriggerExit2D(other);
	}

	void Collider2D::OnLayerChanged(uint32 prev_layer, uint32 new_layer)
	{
		Super::OnLayerChanged(prev_layer, new_layer);

		collision_system_->SetLayerDirty(this);

		DEBUG_LOG("Collider2D::OnLayerChanged()");
	}
}