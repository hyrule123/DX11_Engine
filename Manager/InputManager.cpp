#include "Engine/Core/pch.h"
#include "InputManager.h"

namespace engine
{
	InputManager::InputManager()
	{

	}

	InputManager::~InputManager()
	{

	}

	void InputManager::Update()
	{
		//전에는 안눌렸고, 지금은 눌렸음
		down_keys_ = (~prev_keys_) & (cur_keys_);
		
		//전에는 눌렸는데, 지금은 안눌렸음
		up_keys_ = (prev_keys_) & (~cur_keys_);

		prev_keys_ = cur_keys_;
	}
}