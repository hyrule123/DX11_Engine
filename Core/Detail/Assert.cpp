#include "Engine/Core/pch.h"
#include "Assert.h"

#include <cstdio>
#include <cstdlib>

#include <Windows.h>

namespace engine::detail
{
    namespace
    {
        void EmitAssertText(const std::string& text)
        {
            ::OutputDebugStringA(text.c_str());
            std::fputs(text.c_str(), stderr);
            std::fflush(stderr);
        }

        // 디버거가 없을 때만 호출된다. 확인을 누르면 프로세스를 종료한다.
        void ShowAssertDialog(std::string_view expr, std::string_view message,
            const char* file, int line)
        {
            std::string body;
            body.reserve(256);

            body += std::format("{}\n\n", expr);

            if (!message.empty())
            {
                body += std::format("{}\n\n", message);
            }

            body += std::format("{}({})\n\nTerminating the process.", file, line);

            ::MessageBoxA(nullptr, body.c_str(), "Assertion Failed",
                MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
        }
    }  // namespace

    void ReportAssertFailure(const char* file, int line, const char* expr,
        std::string_view message)
    {
        std::string text;
        text.reserve(256);
        text += "\n";  // 앞선 로그와 분리
        text += "********** ASSERTION FAILED *********\n";
        text += std::format("  expr    : {}\n", expr);

        if (!message.empty())
        {
            text += std::format("  message : {}\n", message);
        }

        // 전체 경로 + (줄) 형식이라 VS 출력 창에서 더블클릭 점프가 동작한다.
        // 테두리 폭을 넘겨 정렬이 깨지더라도 점프 기능을 우선한다.
        text += std::format("{}({})\n", file, line);
        text += "*************************************\n";

        EmitAssertText(text);

        if (::IsDebuggerPresent() == false)
        {
            ShowAssertDialog(expr, message, file, line);
            std::abort();
        }
    }
}  // namespace engine::detail