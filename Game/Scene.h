#pragma once

#include <Engine/Core/Entity.h>

namespace engine
{
	class Scene 
		: public Entity
	{
	public:
		Scene(const std::string_view class_name);
		virtual ~Scene() override;

		virtual void Init() {}
		virtual void Update() {}
		virtual void Render() {}
	};
}