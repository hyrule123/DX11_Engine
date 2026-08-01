#include "Engine/Core/pch.h"
#include "CollisionSystem2D.h"

#include <Engine/Core/Debug.h>

#include <Engine/Game/Component/Collider2D.h>
#include <Engine/Game/GameObject.h>

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
}


namespace engine
{

	CollisionSystem2D::CollisionSystem2D()
	{}

	CollisionSystem2D::~CollisionSystem2D()
	{}
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