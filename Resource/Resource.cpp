#include "Engine/Core/pch.h"
#include "Resource.h"

namespace engine
{
	Resource::Resource(const std::string_view concrete_class_name)
		: Entity(concrete_class_name)
	{
	}
	Resource::~Resource()
	{
	}
}