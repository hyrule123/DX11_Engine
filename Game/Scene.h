#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/ClassMacro.h>

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
		virtual void Update() {}
		virtual void Render() {}

		bool HasInitialized() const { return has_initialized_; }

	private:

		bool has_initialized_ = false;
	};
}