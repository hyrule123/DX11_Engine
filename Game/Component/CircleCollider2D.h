#pragma once
#include <Engine/Game/Component/Collider2D.h>

namespace engine
{
    class CircleCollider2D :
        public Collider2D
    {
        CLASS_INFO(CircleCollider2D, Collider2D)

    public:
        CircleCollider2D();
        virtual ~CircleCollider2D() override;

		virtual void LateUpdate() override;

		virtual AABB2D ComputeWorldBounds() const override;

		void SetRadius(float radius) { radius_ = radius; }
		float GetRadius() const { return radius_; }

        float2 GetCenter() const;

    private:
		float radius_ = 1.0f;
    };
}


