#include "Engine/Core/pch.h"
#include "Resource.h"

namespace engine
{
	Resource::Resource(const HashedStringView& concrete_class_name)
		: Entity(concrete_class_name)
	{
	}
	Resource::~Resource()
	{
	}
}