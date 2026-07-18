#include "Engine/Core/pch.h"
#include "EngineMain.h"

#include <Engine/resource.h>
#include <Engine/Core/Constant.h>

#include <Engine/Manager/GameEngine.h>
#include <Engine/Manager/InputManager.h>

#include <Engine/Core/Debug.h>

#include <windowsx.h>
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

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
        case WM_CLOSE:
            break;
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
    {
        PostQuitMessage(0);
    }
    break;

    case WM_SIZE:
    {
		// 최소화 상태일 때는 리사이즈 처리하지 않음
        if(wParam == SIZE_MINIMIZED)
        {
            break;
        }

		engine::uint32 width = LOWORD(lParam);
		engine::uint32 height = HIWORD(lParam);

		//방어 코드
        if (width > 0 && height > 0)
        {
            engine::EngineMain::GetInst().OnScreenSizeChange((engine::uint32)width, (engine::uint32)height);
        }
    }
    break;
    // 1. 포커스 관리
    case WM_KILLFOCUS:
        // 창이 비활성화될 때 누르고 있던 모든 키를 해제
        engine::InputManager::GetInst().ClearKeys();
        break;

        // ==========================================
        // 2. 키보드 처리
        // ==========================================
    case WM_KEYDOWN: [[fallthrough]];
    case WM_SYSKEYDOWN: // Alt 키 조합 등 시스템 키도 처리
        // lParam의 30번째 비트(0x40000000)는 이전 프레임의 키 상태를 나타냄
        // 이 값이 0일 때만 (즉, 방금 막 눌렸을 때만) 처리하여 OS의 연속 반복 입력을 무시
        if ((lParam & 0x40000000) == 0) {
            engine::InputManager::GetInst().SetKey(wParam, true);
        }
        break;

    case WM_KEYUP: [[fallthrough]];
    case WM_SYSKEYUP:
        engine::InputManager::GetInst().SetKey(wParam, false);
        break;

    // ==========================================
    // 3. 마우스 처리
    // ==========================================
    case WM_MOUSEMOVE: {
        // LOWORD, HIWORD 대신 GET_X_LPARAM을 사용해야 다중 모니터의 음수 좌표를 안전하게 처리함
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        engine::InputManager::GetInst().SetMousePos(x, y);
        break;
    }

    // 마우스 버튼은 가상 키 코드(VK_*)를 그대로 활용하여 키보드 배열에 합치는 것이 관리하기 편함
    case WM_LBUTTONDOWN:
        engine::InputManager::GetInst().SetKey(VK_LBUTTON, true);
        break;
    case WM_LBUTTONUP:
        engine::InputManager::GetInst().SetKey(VK_LBUTTON, false);
        break;

    case WM_RBUTTONDOWN:
        engine::InputManager::GetInst().SetKey(VK_RBUTTON, true);
        break;
    case WM_RBUTTONUP:
        engine::InputManager::GetInst().SetKey(VK_RBUTTON, false);
        break;

    case WM_MBUTTONDOWN:
        engine::InputManager::GetInst().SetKey(VK_MBUTTON, true);
        break;
    case WM_MBUTTONUP:
        engine::InputManager::GetInst().SetKey(VK_MBUTTON, false);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
        //DPI 스케일링 해제
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

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
            ASSERT_RELEASE_MESSAGE(false, "Failed to register window class!");
            return false;
        }

        h_instance_ = hInstance;

        DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

		h_wnd_ = CreateWindowW(kDefaultClassNameW, kDefaultTitleW, window_style,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
        if (!h_wnd_)
        {
			ASSERT_RELEASE_MESSAGE(false, "Failed to create HWND!");
            return false;
        }

		SetScreenSize(kDefaultWidth, kDefaultHeight);
        ShowWindow(h_wnd_, SW_SHOW);
        UpdateWindow(h_wnd_);

        result = GameEngine::GetInst().Init();
        if (false == result)
        {
            ASSERT_RELEASE_MESSAGE(result, "Failed to initialize game engine!");
            return false;
        }

        return true;
	}

    BOOL EngineMain::Run()
    {
        MSG msg = {};

		// Main message loop
        while (is_running_)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                //WMQUIT는 여기서 처리해줘야 함(DispatchMessage를 통해 WndProc로 전달되지 않음.)
                if (WM_QUIT == msg.message)
                {
                    is_running_ = FALSE;
                    break;
                }

                TranslateMessage(&msg);

                //여기서 WndProc 함수를 호출한다.
                DispatchMessage(&msg);
            }
            else
            {
                //게임 메인 루프
                is_running_ = GameEngine::GetInst().Run();
            }
        }

        return is_running_;
    }
    void EngineMain::SetScreenSize(uint32 width, uint32 height)
    {
        // 1. 원하는 순수 도화지 크기 설정
        RECT target_rect = { 0, 0, (LONG)width, (LONG)height };

        // 2. 현재 창의 스타일을 가져와서 테두리/타이틀 바 두께를 더해 크기 보정
        DWORD style = GetWindowLong(h_wnd_, GWL_STYLE);
        AdjustWindowRect(&target_rect, style, FALSE);

        // 3. 보정된 크기 계산
        int new_window_width = target_rect.right - target_rect.left;
        int new_window_height = target_rect.bottom - target_rect.top;

        // 4. SetWindowPos 호출
        SetWindowPos(h_wnd_, nullptr, 0, 0, new_window_width, new_window_height, SWP_NOMOVE | SWP_NOZORDER);
    }
    void EngineMain::OnScreenSizeChange(uint32 width, uint32 height)
    {
        //동일 사이즈는 무시
		if (screen_width_ == width && screen_height_ == height) { return; }

        screen_width_ = width;
        screen_height_ = height;

        if (GameEngine::GetInst().IsInitialized())
        {
			GameEngine::GetInst().OnScreenSizeChange(width, height);
        }
    }
}

