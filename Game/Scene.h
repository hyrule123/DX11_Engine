#pragma once

#include <Engine/Core/Entity.h>
#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Debug.h>

#include <Engine/Collision/CollisionSystem2D.h>

#include <string_view>
#include <vector>

#include <type_traits>
#include <bitset>

namespace engine
{
	class GameObject;
	class Camera;
	class Renderer;

	class Scene 
		: public Entity
	{
		ENTITY_INFO(Scene, Entity)
	public:
		Scene(const HashedStringView& concrete_class_name);
		virtual ~Scene() override;

		virtual void Init() = 0;

		void FrameStart();
		void Update();
		void FixedUpdate();
		void LateUpdate();
		void FrameEnd();

		bool HasInitialized() const { return has_initialized_; }

		//추가 예약, 실제 추가는 FlushPendingGameObjects() 타이밍
		template <typename T> requires std::is_base_of_v<GameObject, T>
		T* AddGameObject() { return AddGameObject(EntityManager::CreateEntity<T>()); }

		GameObject* AddGameObject(const HashedStringView& concrete_class_name);
		GameObject* FindGameObject(const std::string_view name) const;

		CollisionSystem2D* GetCollisionSystem2D() { return &collision_system_2D_; }

		void SetCollisionMask(uint32 layer_a, uint32 layer_b, bool can_collide);

		const std::array<std::bitset<kMaxLayers>, kMaxLayers>& GetCollisionMask() const { return collision_mask_; }

	private:
		GameObject* AddGameObject(u_ptr<GameObject> obj);
		void FlushPending();

		std::vector<u_ptr<GameObject>> game_objects_ = {};

		CollisionSystem2D collision_system_2D_;

		std::array<std::bitset<kMaxLayers>, kMaxLayers> collision_mask_ = {};

		bool has_initialized_ = false;
	};
}