#include "Engine/Core/pch.h"
#include "BlackBoard.h"

namespace engine
{
	BlackBoard::BlackBoard()
		: Super(BlackBoard::kClassConcreteName, BlackBoard::kComponentCategory)
	{}
	BlackBoard::~BlackBoard()
	{}
}