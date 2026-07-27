#include "Engine/Core/pch.h"
#include "Script.h"

namespace engine
{
	Script::Script(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name, Script::kComponentCategory)
	{}
	Script::~Script()
	{}
}