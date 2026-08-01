#pragma once
#include <Engine/Game/Component/Collider2D.h>

namespace engine
{
    class AABBCollider2D :
        public Collider2D
    {
		CLASS_INFO(AABBCollider2D, Collider2D)
	public:
		AABBCollider2D();
		virtual ~AABBCollider2D() override;

		void SetLocalBounds(AABB2D local_bounds) { local_bounds_ = local_bounds; }
		const AABB2D& GetLocalBounds() const { return local_bounds_; }

	protected:
		virtual AABB2D ComputeWorldBounds() const override;

	private:
		AABB2D local_bounds_ = {};
    };
}


