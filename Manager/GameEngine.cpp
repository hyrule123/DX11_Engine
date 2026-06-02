#include "Engine/pch.h"
#include "GameEngine.h"

#include <Engine/Manager/GraphicsDevice.h>

namespace engine
{
	GameEngine::GameEngine()
	{}

	GameEngine::~GameEngine()
	{}

	bool GameEngine::Init()
	{
		if (GraphicsDevice::GetInst().Init() == false)
		{
			ASSERT_RELEASE(false);
			return false;
		}

		return true;
	}

	bool GameEngine::Run()
	{
		return true;
	}
}

