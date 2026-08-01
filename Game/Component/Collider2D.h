#pragma once
#include "Component.h"

#include <Engine/Collision/Collision.h>
#include <Engine/Core/Math.h>

namespace engine
{
	class Collider2D :
		public Component
	{
		CLASS_INFO(Collider2D, Component)
			COMPONENT_CATEGORY(ComponentCategory::kCollider)

	public:
		virtual ~Collider2D() override;

		virtual void Init() override {};
		virtual void Awake() override;
		virtual void OnEnable() override;
		//virtual void FixedUpdate() override;

		virtual void OnLayerChanged(uint32 prev_layer, uint32 new_layer) override;

		void SetTrigger(bool is_trigger) { is_trigger_ = is_trigger; }
		bool IsTrigger() const { return is_trigger_; }

		void SetOffset(float2 offset) { offset_ = offset; }
		float2 GetOffset() const { return offset_; }

		const AABB2D& GetWorldBounds() const;

		void SetCollisionSystemIndex(int32 index) { collision_system_index_ = index; }
		int32 GetCollisionSystemIndex() const { return collision_system_index_; }

		void SetRegisteredLayer(int32 layer) { registered_layer_ = layer; }
		int32 GetRegisteredLayer() const { return registered_layer_; }

		ColliderShape2D GetShape() const { return shape_; }
	protected:
		Collider2D(const HashedStringView& concrete_class_name, ColliderShape2D shape);
		virtual AABB2D ComputeWorldBounds() const = 0;

		Transform* transform_ = {};
		mutable uint32 tf_last_seen_version_ = {};

	private:
		const ColliderShape2D shape_ = {};
		bool   is_trigger_ = {};
		float2 offset_ = {};
		mutable AABB2D world_bounds_ = {};

		// CollisionSystem2D의 배열에 저장된 자신의 Layer과 Index
		int32 registered_layer_ = -1;
		int32 collision_system_index_ = -1;
    };
}

