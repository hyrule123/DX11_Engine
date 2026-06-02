#pragma once

#include <Engine/Singleton.h>

namespace engine
{
	class GameEngine
	{
		DECLARE_SINGLETON(GameEngine)

	public:
		bool Init();

		bool Run();
	};
}