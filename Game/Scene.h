#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/ClassMacro.h>

namespace engine
{
	class Scene 
		: public Entity
	{
		CLASS_INFO(Scene, Entity)
	public:
		Scene(const std::string_view concrete_class_name);
		virtual ~Scene() override;

		virtual void Init() {}
		virtual void Update() {}
		virtual void Render() {}
	};
}