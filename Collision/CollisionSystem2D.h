#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Collision/Collision.h>

#include <Engine/Core/Math.h>

#include <functional>


namespace engine
{
	class Collider2D;

	struct ContactPair2D
	{
		Collider2D* lo;
		Collider2D* hi;
		bool touched_this_frame;
	};

	class CollisionSystem2D
	{
	public:
		CollisionSystem2D();
		~CollisionSystem2D();

		void RegisterCollider(Collider2D* collider);
		void UnregisterCollider(Collider2D* collider);

	private:
		std::array<std::vector<Collider2D*>, kLayerMaxCount> colliders_in_layer_;

		std::unordered_map<ColliderID, ContactPair2D, ColliderID_Hasher> collisions_;
	};
}