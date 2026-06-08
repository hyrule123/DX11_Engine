#pragma once

#include <Engine/Header/Common.h>
#include <Engine/Header/Singleton.h>

namespace engine
{
	class EngineMain
	{
		DECLARE_SINGLETON(EngineMain)

	public:
		bool Init(HINSTANCE hInstance);
		BOOL Run();

		HWND GetHWND() const { return h_wnd_; }

	private:
		HINSTANCE h_instance_ = {};
		HWND h_wnd_ = {};
	};
}


