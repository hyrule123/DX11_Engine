#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>

#include <Engine/Collision/CollisionSystem2D.h>

#include <string_view>
#include <vector>

#include <type_traits>

namespace engine
{
	class GameObject;
	class Camera;
	class Renderer;

	class Scene 
		: public Entity
	{
		CLASS_INFO(Scene, Entity)
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
		void AddGameObject(s_ptr<GameObject> obj);

		template <typename T> requires std::is_base_of_v<GameObject, T>
		s_ptr<T> AddGameObject() {
			s_ptr<T> new_gameobj = std::make_shared<T>();
			AddGameObject(new_gameobj);
			return new_gameobj;
		}

		s_ptr<GameObject> AddGameObject(const HashedStringView& concrete_class_name);

		s_ptr<GameObject> FindGameObject(const std::string_view name) const;

		CollisionSystem2D* GetCollisionSystem2D() { return &collision_system_2D_; }
		
	private:
		void FlushPending();

		std::vector<s_ptr<GameObject>> game_objects_ = {};

		CollisionSystem2D collision_system_2D_ = {};

		bool has_initialized_ = false;
	};
}