#pragma once
#include <Engine/Game/Component/Collider2D.h>

namespace engine
{
    class AABBCollider2D :
        public Collider2D
    {
		ENTITY_INFO(AABBCollider2D, Collider2D)
	public:
		AABBCollider2D();
		virtual ~AABBCollider2D() override;

		virtual void LateUpdate() override;

		void SetLocalBounds(AABB2D local_bounds) { local_bounds_ = local_bounds; }
		void SetSize(float2 size) { local_bounds_.SetSize(size); }
		const AABB2D& GetLocalBounds() const { return local_bounds_; }

	protected:
		virtual AABB2D ComputeWorldBounds() const override;

	private:
		AABB2D local_bounds_ = {};
    };
}


