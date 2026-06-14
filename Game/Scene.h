#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/UtilMacro.h>

#include <vector>

namespace engine
{
	class GameObject;
	class Scene 
		: public Entity
	{
		CLASS_INFO(Scene, Entity)
	public:
		Scene(const std::string_view concrete_class_name);
		virtual ~Scene() override;

		virtual void Init() = 0;
		virtual void FrameStart();
		virtual void Update() {}
		virtual void Render() {}

		bool HasInitialized() const { return has_initialized_; }

		//추가 예약, 실제 추가는 FrameStart() 타이밍
		void AddGameObject(s_ptr<GameObject> obj);

	private:
		std::vector<s_ptr<GameObject>> game_objects_ = {};
		std::vector<s_ptr<GameObject>> pending_add_objects_ = {};

		bool has_initialized_ = false;
	};
}