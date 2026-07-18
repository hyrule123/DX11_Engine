#pragma once

#include <Engine/Core/Singleton.h>
#include <Engine/Core/StdType.h>

namespace engine
{
	class GameEngine
	{
		DECLARE_SINGLETON(GameEngine)

		friend class EngineMain;

	public:
		void OnScreenSizeChange(uint32 width, uint32 height);
		bool IsInitialized() const { return is_initialized_; }

	private:
		bool Init();

		bool Run();

		bool is_initialized_ = {};
	};
}