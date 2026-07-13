#pragma once

#include <Engine/Core/SmartPointer.h>

namespace engine
{
	struct AIContext
	{
		w_ptr<class GameObject> owner = {};
		w_ptr<class HFSM> hfsm = {};
		w_ptr<class Blackboard> blackboard = {};
	};
}