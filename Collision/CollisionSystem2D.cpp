#include "Engine/Core/pch.h"
#include "CollisionSystem2D.h"

#include <Engine/Core/Debug.h>

#include <Engine/Game/Component/Collider2D.h>
#include <Engine/Game/GameObject.h>
#include <Engine/Game/Scene.h>

namespace
{
	using namespace engine;
	class Collider2D;

	bool CheckIntersect_AABB_AABB(
		const Collider2D* _AABB1, const Collider2D* _AABB2, float2* out_contact_point);
	bool CheckIntersect_AABB_OBB(
		const Collider2D* _AABB, const Collider2D* _OBB, float2* out_contact_point);
	bool CheckIntersect_AABB_Circle(
		const Collider2D* _AABB, const Collider2D* _circle, float2* out_contact_point);

	bool CheckIntersect_OBB_OBB(
		const Collider2D* _OBB1, const Collider2D* _OBB2, float2* out_contact_point);
	bool CheckIntersect_OBB_Circle(
		const Collider2D* _OBB, const Collider2D* _circle, float2* out_contact_point);

	inline bool CheckIntersect_OBB_AABB(
		const Collider2D* _OBB, const Collider2D* _AABB, float2* out_contact_point) {
		return CheckIntersect_AABB_OBB(_AABB, _OBB, out_contact_point);
	}

	bool CheckIntersect_Circle_Circle(
		const Collider2D* _circle1, const Collider2D* _circle2, float2* out_contact_point);

	inline bool CheckIntersect_Circle_AABB(
		const Collider2D* _circle, const Collider2D* _AABB, float2* out_contact_point) {
		return CheckIntersect_AABB_Circle(_AABB, _circle, out_contact_point);
	}
	inline bool CheckIntersect_Circle_OBB(
		const Collider2D* _circle, const Collider2D* _OBB, float2* out_contact_point) {
		return CheckIntersect_OBB_Circle(_OBB, _circle, out_contact_point);
	}

	using CheckIntersectFunction =
		bool(*)(const Collider2D*, const Collider2D*, float2*);

	//각 Collider2D 함수 주소를 담고있는 이중 배열
	//각 Collider2D가 가지고 있는 eCollider2D_Type를 index 번호로 해서 함수를 호출하기 위함
	constexpr inline std::array<std::array<CheckIntersectFunction,
		(int)ColliderShape2D::END>, (int)ColliderShape2D::END>
		s_check_intersect_functions = []() {
		using S = ColliderShape2D;
		std::array<std::array<CheckIntersectFunction, (int)S::END>, (int)S::END> arr{};

		arr[(int)S::AABB][(int)S::AABB] = CheckIntersect_AABB_AABB;
		arr[(int)S::AABB][(int)S::OBB] = CheckIntersect_AABB_OBB;
		arr[(int)S::AABB][(int)S::Circle] = CheckIntersect_AABB_Circle; 

		arr[(int)S::OBB][(int)S::AABB] = CheckIntersect_OBB_AABB;
		arr[(int)S::OBB][(int)S::OBB] = CheckIntersect_OBB_OBB;
		arr[(int)S::OBB][(int)S::Circle] = CheckIntersect_OBB_Circle;

		arr[(int)S::Circle][(int)S::AABB] = CheckIntersect_Circle_AABB;
		arr[(int)S::Circle][(int)S::OBB] = CheckIntersect_Circle_OBB;
		arr[(int)S::Circle][(int)S::Circle] = CheckIntersect_Circle_Circle;

		return arr;
	}();

	int32_2 ConvertWorldPosToCellIndex(float2 pos, float2 cell_size_inv) {
		float2 cell_index_f = pos * cell_size_inv;
		return {static_cast<int32>(std::floor(cell_index_f.x)), static_cast<int32>(std::floor(cell_index_f.y))};
	}

	uint32 GetBucketIndex(int32 x, int32 y, uint64 bucket_bit_shifts) {
		uint64 k = (static_cast<uint64>(static_cast<uint32>(y)) << 32ull) | static_cast<uint64>(static_cast<uint32>(x));

		// splitmix64 finalizer
		k ^= k >> 30; 
		k *= 0xBF58476D1CE4E5B9ULL;
		k ^= k >> 27; 
		k *= 0x94D049BB133111EBULL;
		k ^= k >> 31;

		return static_cast<uint32>(k >> (64 - bucket_bit_shifts));   // 상위 비트 취득
	}

	bool CheckIntersect_AABB_AABB(const AABB2D& a, const AABB2D& b)
	{
		const bool x_overlap = (a.left_bottom.x <= b.right_top.x) && (a.right_top.x >= b.left_bottom.x);
		const bool y_overlap = (a.left_bottom.y <= b.right_top.y) && (a.right_top.y >= b.left_bottom.y);
		return x_overlap && y_overlap;
	}
}


namespace engine
{

	CollisionSystem2D::CollisionSystem2D(Scene* owner_scene)
		: owner_scene_(owner_scene)
	{}

	CollisionSystem2D::~CollisionSystem2D()
	{}
	void CollisionSystem2D::Init()
	{
		if (bucket_.empty())
		{
			SetBucketSize(kDefaultCollisionBucketSize);
		}
	}
	void CollisionSystem2D::SetBucketSize(uint64 requested_size)
	{
		//클램핑: 최소 2, 최대 kMaxCollisionBucketSize
		const uint64 clamped = std::clamp(requested_size, 2ull, kMaxCollisionBucketSize);

		// 알아서 가장 가까운 2의 거듭제곱으로 올려줌 (예: 2000 -> 2048)
		bucket_size_ = std::bit_ceil(clamped);

		// 버킷 배열 크기 조정 
		// Start Index이므로 [0] = 0이어야 함. -> 한 칸 더 필요!
		bucket_start_idx_.resize(bucket_size_ + 1);
		bucket_cursor_.resize(bucket_size_ + 1);

		// 비트시프트 계산
		bucket_size_bit_shifts_ = 64ull - std::countr_zero(bucket_size_);
	}
	void CollisionSystem2D::RegisterCollider(Collider2D* collider)
	{
		ASSERT(collider);

		int32 registered_layer = collider->GetRegisteredLayer();
		int32 index = collider->GetCollisionSystemIndex();
		ASSERT_MESSAGE(registered_layer == -1 && index == -1, "Collider is already registered");

		uint32 layer = collider->GetOwnerGameObject()->GetLayer();
		colliders_in_layer_[layer].push_back(collider);

		// Collider2D의 배열에 저장된 자신의 layer과 index를 저장시킴
		collider->SetRegisteredLayer((int32)layer);
		collider->SetCollisionSystemIndex((int32)(colliders_in_layer_[layer].size() - 1));
	}

	void CollisionSystem2D::UnregisterCollider(Collider2D* collider)
	{
		ASSERT(collider);

		int32 registered_layer = collider->GetRegisteredLayer();
		int32 index = collider->GetCollisionSystemIndex();
		ASSERT_MESSAGE(index != -1 && registered_layer != -1, "Collider is not registered");

		// 저장된 포인터 주소가 다르면 뭔가 문제 있는거임
		ASSERT(colliders_in_layer_[registered_layer][index] == collider);

		//맨 뒤에 있는 collider를 현재 index로 옮기고, 맨 뒤에 있는 collider를 pop_back()으로 제거
		colliders_in_layer_[registered_layer][index] = colliders_in_layer_[registered_layer].back();
		colliders_in_layer_[registered_layer][index]->SetCollisionSystemIndex(index);
		colliders_in_layer_[registered_layer].pop_back();

		// 원상복구
		collider->SetCollisionSystemIndex(-1);
		collider->SetRegisteredLayer(-1);
	}

	void CollisionSystem2D::FixedUpdate()
	{
		//TODO: 충돌 감지 기간 동안에는 Collider 추가/제거 금지 가드 필요

		uint32 colliders_count = 0;
		for (const auto& layer : colliders_in_layer_)
		{
			colliders_count += (uint32)layer.size();
		}

		// 평균적으로 4개의 bucket에 걸쳐있다고 가정
		staged_entries_.reserve(colliders_count * 4); 

		//bucket_start_idx_ 초기화
		std::fill(bucket_start_idx_.begin(), bucket_start_idx_.end(), 0);

		// Pass 1: Staging buffer 채워넣기
		for(uint32 i = 0; i < (uint32)colliders_in_layer_.size(); ++i)
		{
			//not null이 보장됨
			for (Collider2D* col : colliders_in_layer_[i])
			{
				const AABB2D& bounds = col->GetWorldBounds();

				int32_2 cell_index_LB = ConvertWorldPosToCellIndex(bounds.left_bottom, cell_size_inv_);

				int32_2 cell_index_RT = ConvertWorldPosToCellIndex(bounds.right_top, cell_size_inv_);

				const int64 cells = int64(cell_index_RT.x - cell_index_LB.x + 1) * (cell_index_RT.y - cell_index_LB.y + 1);
				ASSERT_MESSAGE(cells <= kMaxCellsPerCollider, "콜라이더가 너무 많은 셀에 걸침");

				for (int32 x = cell_index_LB.x; x <= cell_index_RT.x; ++x)
				{
					for (int32 y = cell_index_LB.y; y <= cell_index_RT.y; ++y)
					{
						uint32 bucket_index = GetBucketIndex(x, y, bucket_size_bit_shifts_);
						staged_entries_.emplace_back(
							GridEntry{
							x,
							y,
							bucket_index,
							(uint32)i,
							col
							}
						);

						// 각 Bucket의 갯수를 카운트한다.
						// 차후 '누적합' 진행 예정, 
						// bucket의 start index이므로, [0] = 0이어야 함
						// -> bucket_index + 1에 카운트 증가시켜야 함!
						bucket_start_idx_[bucket_index + 1]++; 
					}
				}
			}
		}

		// Pass 2: 누적합 계산
		for (uint32 i = 1; i < bucket_start_idx_.size(); ++i)
		{
			bucket_start_idx_[i] += bucket_start_idx_[i - 1];
		}
		bucket_cursor_ = bucket_start_idx_; // cursor_ 초기화

		// Pass 3: bucket_ 채워넣기
		bucket_.resize(staged_entries_.size());
		for (const auto& entry : staged_entries_)
		{
			const uint32 bucket_index = entry.bucket_index;
			const uint32 insert_index = bucket_cursor_[bucket_index];
			bucket_cursor_[bucket_index]++; // 다음에 들어올 entry를 위해 cursor_ 증가

			bucket_[insert_index] = entry;
		}
		staged_entries_.clear(); // 임시 버퍼 비우기

#ifdef _DEBUG
		//전부 제대로 작성되었는지 확인 절차
		for (size_t i = 0; i < bucket_.size(); ++i)
		{
			ASSERT(bucket_cursor_[i] == bucket_start_idx_[i + 1]);
		}
#endif

		// Pass 4: Broad Phase

		// 레이어 마스크 (SSOT - Scene에서 관리)
		const std::array<std::bitset<kMaxLayers>, kMaxLayers>& collision_mask = owner_scene_->GetCollisionMask();

		for (size_t bucket_idx = 0; bucket_idx < bucket_size_; ++bucket_idx) {
			const uint32 begin = bucket_start_idx_[bucket_idx];
			const uint32 end = bucket_start_idx_[bucket_idx + 1];

			//같은 셀 내의 Collider2D들끼리 충돌 감지
			for (uint32 i = begin; i < end; ++i) {
				const GridEntry& e_i = bucket_[i];
				const AABB2D& e_i_world_bound = e_i.collider->GetWorldBounds();

				for (uint32 j = i + 1; j < end; ++j) {
					const GridEntry& e_j = bucket_[j];
					const AABB2D& e_j_world_bound = e_j.collider->GetWorldBounds();

					// 1. 해시 충돌로 섞인 다른 셀 배제
					if (e_i.cell_x != e_j.cell_x || e_i.cell_y != e_j.cell_y) { continue; }

					// 2. 레이어 마스크 확인
					if (false == collision_mask[e_i.layer][e_j.layer]) { continue; }

					// 3. AABB 겹침 확인, 안 겹칠 시 early-out
					if(false == CheckIntersect_AABB_AABB(
						e_i_world_bound,
						e_j_world_bound
					)) 
					{ continue; }

					// 4. min-cell 규칙 (중복 쌍 제거)
					// 겹침이 확인되었을 경우 - 겹치는 영역의 Left Bottom Index를 계산 후
					// 해당 인덱스와 동일한 셀에서만 처리 진행
					float2 overlap_left_bottom = float2(
						std::max(e_i_world_bound.left_bottom.x, e_j_world_bound.left_bottom.x),
						std::max(e_i_world_bound.left_bottom.y, e_j_world_bound.left_bottom.y)
					);

					int32_2 overlap_cell_index = ConvertWorldPosToCellIndex(overlap_left_bottom, cell_size_inv_);

					if (overlap_cell_index.x != e_i.cell_x || overlap_cell_index.y != e_i.cell_y) 
					{ continue; }


					// 5. Narrow Phase
				}
			}
		}

	}
	void CollisionSystem2D::SetCellSize(float2 cell_size)
	{
		ASSERT(cell_size.x >= 1.0f && cell_size.y >= 1.0f);
		cell_size_ = cell_size;
		cell_size_inv_ = float2(1.0f / cell_size.x, 1.0f / cell_size.y);
	}

}

namespace 
{
	bool CheckIntersect_AABB_AABB(const Collider2D* _AABB1, const Collider2D* _AABB2, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_AABB_OBB(const Collider2D* _AABB, const Collider2D* _OBB, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_AABB_Circle(const Collider2D* _AABB, const Collider2D* _circle, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_OBB_OBB(const Collider2D* _OBB1, const Collider2D* _OBB2, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_OBB_Circle(const Collider2D* _OBB, const Collider2D* _circle, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_Circle_Circle(const Collider2D* _circle1, const Collider2D* _circle2, float2* out_contact_point)
	{
		return false;
	}
}