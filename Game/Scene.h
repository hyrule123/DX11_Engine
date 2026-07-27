#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>

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
		void LateUpdate();
		void FrameEnd();

		bool HasInitialized() const { return has_initialized_; }

		//추가 예약, 실제 추가는 FrameStart() 타이밍
		void AddGameObject(s_ptr<GameObject> obj);

		template <typename T> requires std::is_base_of_v<GameObject, T>
		s_ptr<T> AddGameObject() {
			s_ptr<T> new_gameobj = std::make_shared<T>();
			AddGameObject(new_gameobj);
			return new_gameobj;
		}

		s_ptr<GameObject> AddGameObject(const HashedStringView& concrete_class_name);
		
	private:
		std::vector<s_ptr<GameObject>> game_objects_ = {};
		std::vector<s_ptr<GameObject>> pending_add_objects_ = {};

		bool has_initialized_ = false;
	};
}