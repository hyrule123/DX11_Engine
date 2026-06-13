#include "Engine/Core/pch.h"
#include "Script.h"

namespace engine
{
	Script::Script()
		: Super(CLASS_TO_STRING(Script), Script::kComponentCategory)
	{}
	Script::~Script()
	{}
}