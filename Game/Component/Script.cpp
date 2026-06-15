#include "Engine/Core/pch.h"
#include "Script.h"

namespace engine
{
	Script::Script(const std::string_view class_name)
		: Super(class_name, Script::kComponentCategory)
	{}
	Script::~Script()
	{}
}