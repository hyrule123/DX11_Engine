#include "Engine/Core/pch.h"
#include "Blackboard.h"

namespace engine
{
	Blackboard::Blackboard()
		: Super(Blackboard::kClassConcreteName, Blackboard::kComponentCategory)
	{}
	Blackboard::~Blackboard()
	{}
}