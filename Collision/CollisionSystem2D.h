#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>

#include <Engine/Collision/Collision.h>

#include <functional>
#include <array>

namespace engine
{
	class Collider2D;
	class Scene;

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
		struct ContactPair2D
		{
			Collider2D* lo;
			Collider2D* hi;
			bool touched_this_step_;
			bool was_trigger_;
		};

		struct GridEntry {
			//For Broad Phase
			int32 cell_x;
			int32 cell_y;
			uint32 bucket_index;
			uint32 layer;

			//For Narrow Phase
			Collider2D* collider;
		};

		Scene* owner_scene_ = {};

		std::array<std::vector<Collider2D*>, kMaxLayers> colliders_in_layer_;

		// 1단계 과정에서 만들어 둔 GridEntry를 담아두는 임시 버퍼
		std::vector<GridEntry> staged_entries_;

		// 버킷 별 Start Index
		std::vector<uint32> bucket_start_idx_;
		// 버킷에 데이터 작성을 위한 cursor
		std::vector<uint32> bucket_cursor_;
		// 버킷에 실제로 들어가는 데이터(flat hash table)
		std::vector<GridEntry> bucket_;

		size_t bucket_size_ = {};
		uint64 bucket_size_bit_shifts_ = {};

		float2 cell_size_ = { 1.0f, 1.0f };
		float2 cell_size_inv_ = { 1.0f, 1.0f };

		std::unordered_map<ColliderID, ContactPair2D, ColliderID_Hasher> collisions_;
	};
}