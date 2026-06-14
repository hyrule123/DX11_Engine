#include "Engine/Core/pch.h"
#include "Script.h"

namespace engine
{
	Script::Script()
		: Super(STRINGIFY(Script), Script::kComponentCategory)
	{}
	Script::~Script()
	{}
}