#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/Windows.h>

#include <Engine/Core/StdType.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace engine
{
	class EngineMain
	{
		DECLARE_SINGLETON(EngineMain)

		friend LRESULT CALLBACK ::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		bool Init(HINSTANCE hInstance);
		BOOL Run();

		HWND GetHWND() const { return h_wnd_; }

		void SetScreenSize(uint32 width, uint32 height);
		uint32 GetScreenWidth() const { return screen_width_; }
		uint32 GetScreenHeight() const { return screen_height_; }

	private:
		// WndProc 호출 용도
		void OnScreenSizeChange(uint32 width, uint32 height);

		HINSTANCE h_instance_ = {};
		HWND h_wnd_ = {};
		BOOL is_running_ = TRUE;

		uint32 screen_width_ = {};
		uint32 screen_height_ = {};
	};
}


