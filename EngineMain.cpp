#include "Engine/pch.h"
#include "EngineMain.h"

#include <Engine/resource.h>
#include <Engine/Constant.h>

namespace
{
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }
}

namespace engine
{
	EngineMain::EngineMain()
	{
    }
	EngineMain::~EngineMain()
	{
        
    }
	bool EngineMain::Init(HINSTANCE hInstance)
	{
        WNDCLASSEXW wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = TEXT("DX11_Engine_Class");
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

        bool result = static_cast<bool>(RegisterClassExW(&wcex));
        if (!result)
        {
            ASSERT_MESSAGE_RELEASE(false, "Failed to register window class!");
            return false;
        }

        h_instance_ = hInstance;

		h_wnd_ = CreateWindowW(kDefaultClassNameW, kDefaultTitleW, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
        if (!h_wnd_)
        {
			ASSERT_MESSAGE_RELEASE(false, "Failed to create HWND!");
            return false;
        }

        ShowWindow(h_wnd_, SW_SHOW);
        UpdateWindow(h_wnd_);

        return true;
	}

    BOOL EngineMain::Run()
    {
        BOOL is_running = TRUE;

        MSG msg = {};

		// Main message loop
        while (is_running)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                //WMQUIT는 여기서 처리해줘야 함(DispatchMessage를 통해 WndProc로 전달되지 않음.)
                if (WM_QUIT == msg.message)
                {
                    is_running = FALSE;
                    break;
                }

                TranslateMessage(&msg);

                //여기서 WndProc 함수를 호출한다.
                DispatchMessage(&msg);
            }
            else
            {
                //게임 메인 루프는 여기서 돌아감.
                is_running = TRUE;
            }
        }

        return is_running;
    }
}

