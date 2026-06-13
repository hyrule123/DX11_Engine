#include "Engine/Core/pch.h"
#include "Component.h"

namespace engine
{
	Component::Component(const std::string_view concrete_class_name, ComponentCategory category)
		: Entity(concrete_class_name)
		, category_(category)
	{}

	Component::~Component()
	{}
}

