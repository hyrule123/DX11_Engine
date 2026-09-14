#include "Engine/Core/pch.h"
#include "EngineLog.h"

#include <cstdio>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <system_error>


namespace engine::log
{
    static_assert(sizeof(HRESULT) == sizeof(HResultType), "헤더에서 HResult 파일 크기를 확인하고 맞출것.");

    namespace
    {
        void Emit(const char* text) noexcept
        {
            ::OutputDebugStringA(text);
            std::fputs(text, stderr);
        }
    }

    void OnLogMessage(const std::string& message)
    {
        const std::string text = std::format("[LOG] {}\n", message);
        Emit(text.c_str());
    }

    bool OnErrorMessage(const char* file, int line, const std::string& message)
    {
        // 로그는 항상 남긴다. 대화상자를 닫아버려도 기록은 출력 창에 있다.
        const std::string text = std::format(
            "**********\n[ERROR] {}\n{}({})\n**********\n", message, file, line);
        Emit(text.c_str());

        // 원인과 위치를 반드시 함께 보여준다. 메시지만 띄우면 알려주는 의미가 없다.
        ::MessageBoxA(nullptr, text.c_str(), "Error",
            MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);

        // 디버거가 붙어 있으면 호출 지점에서 멈춘다.
        return ::IsDebuggerPresent() != FALSE;
    }

    bool HResultErrorMessage(const char* file, int line, HResultType hresult)
    {
        const std::string err = std::format("HRESULT = 0x{:08X}: {}", (unsigned)hresult, std::system_category().message(hresult));
        return OnErrorMessage(file, line, err);
    }
}