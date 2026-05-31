#pragma once

#include "Engine/Common.h"

#include "Engine/Singleton.h"

namespace engine
{
	class EngineMain
	{
	public:
		EngineMain();
		~EngineMain();

	private:
		EngineMain(const EngineMain&) = delete;
		EngineMain(EngineMain&&) = delete;

	public:
		void Init(HINSTANCE hInstance, HICON windowIcon);

	private:
		HINSTANCE h_instance_ = {};
		HICON win_icon_ = {};
	};
}


