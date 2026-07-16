#pragma once

#include <Engine/Core/SmartPointer.h>

namespace engine
{
	struct AIContext
	{
		w_ptr<class GameObject> game_object = {};
		w_ptr<class BlackBoard> black_board = {};
		w_ptr<class Transform> transform = {};
	};
}