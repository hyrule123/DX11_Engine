#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/Windows.h>

#include <Engine/Core/CoreTypes.h>

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

		void SetScreenSize(uint32_t width, uint32_t height);
		uint32_t GetScreenWidth() const { return screen_width_; }
		uint32_t GetScreenHeight() const { return screen_height_; }

		void SetCursorClip(bool enable);

	private:
		void ApplyCursorClip();

		// WndProc 호출 용도
		void OnScreenSizeChange(uint32_t width, uint32_t height);
		void OnActivate(bool active);

		HINSTANCE h_instance_ = {};
		HWND h_wnd_ = {};
		BOOL is_running_ = TRUE;

		uint32_t screen_width_ = {};
		uint32_t screen_height_ = {};

		bool cursor_clip_enabled_ = {};
		bool is_cursor_clipped_ = {};	// 이 창이 ClipCursor를 건 상태
		bool is_wnd_active_ = {};
	};
}


