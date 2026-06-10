#pragma once

#include <Engine/Core/Singleton.h>

namespace engine
{
	class GameEngine
	{
		DECLARE_SINGLETON(GameEngine)

		friend class EngineMain;

	private:
		bool Init();

		bool Run();
	};
}