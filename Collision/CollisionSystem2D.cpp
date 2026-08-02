#include "Engine/Core/pch.h"
#include "CollisionSystem2D.h"

#include <Engine/Core/Debug.h>

#include <Engine/Game/Component/Collider2D.h>
#include <Engine/Game/GameObject.h>
#include <Engine/Game/Scene.h>

// Intersect 함수 선언부
namespace
{
	using namespace engine;
	class ::engine::Collider2D;

	bool CheckIntersect_AABB_AABB(
		const Collider2D* AABB1, const Collider2D* AABB2, float2* out_contact_point);
	bool CheckIntersect_AABB_OBB(
		const Collider2D* AABB, const Collider2D* OBB, float2* out_contact_point);
	bool CheckIntersect_AABB_Circle(
		const Collider2D* AABB, const Collider2D* circle, float2* out_contact_point);

	bool CheckIntersect_OBB_OBB(
		const Collider2D* OBB1, const Collider2D* OBB2, float2* out_contact_point);
	bool CheckIntersect_OBB_Circle(
		const Collider2D* OBB, const Collider2D* circle, float2* out_contact_point);

	inline bool CheckIntersect_OBB_AABB(
		const Collider2D* OBB, const Collider2D* AABB, float2* out_contact_point) {
		return CheckIntersect_AABB_OBB(AABB, OBB, out_contact_point);
	}

	bool CheckIntersect_Circle_Circle(
		const Collider2D* circle1, const Collider2D* circle2, float2* out_contact_point);

	inline bool CheckIntersect_Circle_AABB(
		const Collider2D* circle, const Collider2D* AABB, float2* out_contact_point) {
		return CheckIntersect_AABB_Circle(AABB, circle, out_contact_point);
	}
	inline bool CheckIntersect_Circle_OBB(
		const Collider2D* circle, const Collider2D* OBB, float2* out_contact_point) {
		return CheckIntersect_OBB_Circle(OBB, circle, out_contact_point);
	}

	using CheckIntersectFunction =
		bool(*)(const Collider2D*, const Collider2D*, float2*);

	//각 Collider2D 함수 주소를 담고있는 이중 배열
	//각 Collider2D가 가지고 있는 eCollider2D_Type를 index 번호로 해서 함수를 호출하기 위함
	constexpr inline std::array<std::array<CheckIntersectFunction,
		(int)ColliderShape2D::END>, (int)ColliderShape2D::END>

		s_check_intersect_functions = []() { // <<< 요기 변수명

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
		bucket_size_bit_shifts_ = std::countr_zero(bucket_size_);
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
					float2 contact_point = {};
					
					bool is_trigger = e_i.collider->IsTrigger() || e_j.collider->IsTrigger();

					// Trigger 시 contact_point 계산 필요 없음, nullptr 전달
					float2* contact_point_ptr = is_trigger ? nullptr : &contact_point;

					bool is_intersect = s_check_intersect_functions
						[(int)e_i.collider->GetShape()][(int)e_j.collider->GetShape()]
						(
						e_i.collider,
						e_j.collider,
						contact_point_ptr
						);

					const uint32 collider_id_i = e_i.collider->GetInstanceID();
					const uint32 collider_id_j = e_j.collider->GetInstanceID();

					ColliderID id_pair{ collider_id_i, collider_id_j };

					if (is_intersect)
					{
						ContactPair2D pair;

						if (collider_id_i == id_pair.GetLo())
						{
							pair.lo = e_i.collider;
							pair.hi = e_j.collider;
						}
						else
						{
							pair.lo = e_j.collider;
							pair.hi = e_i.collider;
						}
						pair.touched_this_step_ = false;
						pair.was_trigger_ = is_trigger;

						// unordered_map에 삽입 시도, 이미 존재하면 삽입 실패
						auto [it, inserted] = collisions_.try_emplace(id_pair , pair);

						// 삽입 성공 = 첫 충돌
						if (inserted)
						{
							if (it->second.was_trigger_)
							{
								it->second.lo->TriggerEnter2D(it->second.hi);
								it->second.hi->TriggerEnter2D(it->second.lo);
							}
							else
							{
								Collision2D col_info;
								col_info.contact_point = contact_point;

								col_info.other_collider = it->second.hi;
								it->second.lo->CollisionEnter2D(col_info);

								col_info.other_collider = it->second.lo;
								it->second.hi->CollisionEnter2D(col_info);
							}
						}
						// 삽입 실패 = 이미 존재하는 쌍
						else
						{
							if (it->second.was_trigger_)
							{
								it->second.lo->TriggerStay2D(it->second.hi);
								it->second.hi->TriggerStay2D(it->second.lo);
							}
							else
							{
								Collision2D col_info;
								col_info.contact_point = contact_point;

								col_info.other_collider = it->second.hi;
								it->second.lo->CollisionStay2D(col_info);

								col_info.other_collider = it->second.lo;
								it->second.hi->CollisionStay2D(col_info);
							}
						}
						// 이번 step에 touch되었음을 표시
						it->second.touched_this_step_ = true;
					}

					// OnExit는 Pass 5에서 처리한다.
				}
			}
		}

		// Pass 5: OnExit 처리
		for (auto it = collisions_.begin(); it != collisions_.end(); ) {
			ContactPair2D& pair = it->second;

			// 다음 스텝을 위해 리셋
			if (pair.touched_this_step_) {
				pair.touched_this_step_ = false;      
				++it;
				continue;
			}

			// 이번 step에 touch되지 않은 쌍 = OnExit 발생
			if (pair.was_trigger_) {
				pair.lo->TriggerExit2D(pair.hi);
				pair.hi->TriggerExit2D(pair.lo);
			}
			else {
				pair.lo->CollisionExit2D(pair.hi);
				pair.hi->CollisionExit2D(pair.lo);
			}
			it = collisions_.erase(it);
		}
	}
	void CollisionSystem2D::SetCellSize(float2 cell_size)
	{
		ASSERT(cell_size.x >= 1.0f && cell_size.y >= 1.0f);
		cell_size_ = cell_size;
		cell_size_inv_ = float2(1.0f / cell_size.x, 1.0f / cell_size.y);
	}
}

#include <Engine/Game/Component/AABBCollider2D.h>
#include <Engine/Game/Component/CircleCollider2D.h>

// Intersect 함수 정의부
namespace 
{
	using namespace engine;
	class ::engine::Collider2D;

	bool CheckIntersect_AABB_AABB(const Collider2D* in_aabb1, const Collider2D* in_aabb2,
		float2* out_contact_point)
	{
		ASSERT(in_aabb1 && in_aabb2);

		const AABB2D& bound_1 = in_aabb1->GetWorldBounds();
		const AABB2D& bound_2 = in_aabb2->GetWorldBounds();

		if (!CheckIntersect_AABB_AABB(bound_1, bound_2))
			return false;

		if (out_contact_point)
		{
			const AABB2D contact_area{
				{ std::max(bound_1.left_bottom.x, bound_2.left_bottom.x),
				  std::max(bound_1.left_bottom.y, bound_2.left_bottom.y) },
				{ std::min(bound_1.right_top.x,   bound_2.right_top.x),
				  std::min(bound_1.right_top.y,   bound_2.right_top.y) }
			};
			*out_contact_point = contact_area.GetCenter();
		}
		return true;
	}
	bool CheckIntersect_AABB_OBB(const Collider2D* AABB, const Collider2D* OBB, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_AABB_Circle(const Collider2D* AABB, const Collider2D* circle, float2* out_contact_point)
	{
		ASSERT(AABB && circle);
		ASSERT(AABB->GetShape() == ColliderShape2D::AABB && circle->GetShape() == ColliderShape2D::Circle);

		const AABBCollider2D* aabb = static_cast<const AABBCollider2D*>(AABB);
		const CircleCollider2D* cc = static_cast<const CircleCollider2D*>(circle);

		const float r = cc->GetRadius();
		const float2 circle_center = cc->GetCenter();
		const AABB2D& bounds = aabb->GetWorldBounds();

		// 가장 가까운 점을 구함(clamp 사용 시 바로 구하기 가능)
		float2 closest = circle_center;
		closest.Clamp(bounds.left_bottom, bounds.right_top);

		// 가장 가까운 꼭지점에서 원의 중심까지의 거리 제곱이 반지름 제곱보다 작거나 같으면 충돌
		const float dist_sq = (circle_center - closest).LengthSquared();

		const bool is_intersecting = dist_sq <= r * r;

		if (is_intersecting && out_contact_point)
		{
			const float dist = std::sqrt(dist_sq);

			// 원의 중심이 AABB 외부에 있을 때
			if (dist > kEpsilon)
			{
				*out_contact_point = closest;
			}
			// 원의 중심이 AABB 내부에 있을 때
			// closest point가 원의 중심과 동일, 탈출 비용이 가장 싼 변 위의 점을 찾아야 함
			else
			{
				const float to_left = circle_center.x - bounds.left_bottom.x;
				const float to_right = bounds.right_top.x - circle_center.x;
				const float to_bottom = circle_center.y - bounds.left_bottom.y;
				const float to_top = bounds.right_top.y - circle_center.y;

				float min_dist = to_left;
				float2 face_point = { bounds.left_bottom.x, circle_center.y };
				if (to_right < min_dist) { min_dist = to_right;  face_point = { bounds.right_top.x,   circle_center.y }; }
				if (to_bottom < min_dist) { min_dist = to_bottom; face_point = { circle_center.x, bounds.left_bottom.y }; }
				if (to_top < min_dist) { min_dist = to_top;    face_point = { circle_center.x, bounds.right_top.y }; }

				*out_contact_point = face_point;   // 탈출 비용이 가장 싼 변 위의 점
			}
		}

		return is_intersecting;
	}
	bool CheckIntersect_OBB_OBB(const Collider2D* OBB1, const Collider2D* OBB2, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_OBB_Circle(const Collider2D* OBB, const Collider2D* circle, float2* out_contact_point)
	{
		return false;
	}
	bool CheckIntersect_Circle_Circle(const Collider2D* circle1, const Collider2D* circle2, float2* out_contact_point)
	{
		ASSERT(circle1 && circle2);
		ASSERT(circle1->GetShape() == ColliderShape2D::Circle && circle2->GetShape() == ColliderShape2D::Circle);

		const CircleCollider2D* c1 = static_cast<const CircleCollider2D*>(circle1);
		const CircleCollider2D* c2 = static_cast<const CircleCollider2D*>(circle2);

		const float2 center1 = c1->GetCenter();
		const float2 center2 = c2->GetCenter();

		const float2 diff = center2 - center1;
		const float distance_squared = diff.LengthSquared();
		const float radius_sum = c1->GetRadius() + c2->GetRadius();

		const bool is_intersecting = distance_squared <= radius_sum * radius_sum;
		if (out_contact_point && is_intersecting)
		{
			const float distance = std::sqrt(distance_squared);
			const float2 direction = distance > kEpsilon ? (diff / distance) : float2(1.0f, 0.0f); // 단위 벡터
			const float penetration_depth = radius_sum - distance; // 원의 침투 깊이

			//원의 침투 깊이의 절반만큼 이동
			// 큰 원이 작은 원 안으로 들어갔을 때의 오차를 대비하여 clamp 사용
			const float offset = std::clamp(c1->GetRadius() - penetration_depth * 0.5f, 0.0f, distance);

			*out_contact_point = center1 + direction * offset;
		}
		return is_intersecting;
	}
}