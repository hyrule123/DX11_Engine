#pragma once

#include <Engine/Core/SmartPointer.h>

namespace engine
{
	struct AIContext
	{
		class GameObject* game_object = {};
		class BlackBoard* black_board = {};
		class Transform* transform = {};
	};
}