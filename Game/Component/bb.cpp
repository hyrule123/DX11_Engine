#include "Engine/Core/pch.h"
#include "BlackBoard.h"

namespace engine
{
	Blackboard::Blackboard()
		: Super(Blackboard::kClassConcreteName, Blackboard::kComponentCategory)
	{}
	Blackboard::~Blackboard()
	{}
}