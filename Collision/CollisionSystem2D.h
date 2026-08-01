#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>

#include <Engine/Collision/Collision.h>

#include <functional>

namespace engine
{
	class Collider2D;

	struct ContactPair2D
	{
		Collider2D* lo;
		Collider2D* hi;
		bool touched_this_step_;
	};

	class CollisionSystem2D
	{
	public:
		CollisionSystem2D(Scene* owner_scene);
		~CollisionSystem2D();

		void Init();

		void SetBucketSize(uint64 requested_size);

		void RegisterCollider(Collider2D* collider);
		void UnregisterCollider(Collider2D* collider);

		void FixedUpdate();

		void SetCellSize(float2 cell_size);
	private:
		Scene* owner_scene_ = {};

		struct GridEntry {
			//For Broad Phase
			int32 cell_x;
			int32 cell_y;
			uint32 bucket_index;
			uint32 layer;

			//For Narrow Phase
			Collider2D* collider;
		};

		std::array<std::vector<Collider2D*>, kMaxLayers> colliders_in_layer_;

		// 1단계 과정에서 만들어 둔 GridEntry를 담아두는 임시 버퍼
		std::vector<GridEntry> staged_entries_;

		//버킷 별 Start Index
		std::vector<uint32> bucket_start_idx_;
		std::vector<uint32> bucket_cursor_;
		std::vector<GridEntry> bucket_;

		size_t bucket_size_ = {};
		uint64 bucket_size_bit_shifts_ = {};

		float2 cell_size_ = { 1.0f, 1.0f };
		float2 cell_size_inv_ = { 1.0f, 1.0f };

		std::unordered_map<ColliderID, ContactPair2D, ColliderID_Hasher> collisions_;
	};
}