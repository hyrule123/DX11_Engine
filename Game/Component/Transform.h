#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Game/Component/ComponentCategory.h>
#include <Engine/Core/ClassMacro.h>

namespace engine
{
	class Transform final :
		public Component
	{
		CLASS_INFO(Transform, Component)
		COMPONENT_CATEGORY(ComponentCategory::kTransform)
	public:
		Transform();
		virtual ~Transform() override;
		
		virtual void Init() override;
	};
}


