#pragma once
#include "Assert.h"

namespace engine::log
{
    // 로그 한 줄 출력. 게이트는 매크로 쪽에서 이미 통과한 상태로 들어온다.
    void OnLogMessage(const std::string& message);

    // MessageBox를 무조건 표시한다.
    // 반환값은 "호출 지점에서 멈춰야 하는가" = 디버거 부착 여부.
    // ENGINE_DEBUG_BREAK는 실패한 줄에서 멈추도록 매크로 쪽에 둔다 (ASSERT와 동일).
    [[nodiscard]] bool OnErrorMessage(const char* file, int line, const std::string& message);

    // 포맷 문자열까지 __VA_ARGS__ 안에 들어오므로 trailing comma 문제가 없다.
    // std::format_string<Args...>가 컴파일 타임에 포맷/인자 타입을 검증한다.
    template <typename... Args>
    [[nodiscard]] std::string FormatString(std::format_string<Args...> fmt, Args&&... args)
    {
        return std::format(fmt, std::forward<Args>(args)...);
    }

    // 꺼진 빌드용. if constexpr (false) 안에서 호출되어 런타임 비용은 0이지만
    // 포맷 문자열 검증과 인자 사용 표시는 그대로 유지된다.
    template <typename... Args>
    void Unused(std::format_string<Args...> fmt, const Args&... args) noexcept
    {
        (void)fmt;
        ((void)args, ...);
    }

    using HResultType = long;
    bool HResultErrorMessage(const char* file, int line, HResultType hresult);
}

// -----------------------------------------------------------------------------
// 켜진 몸통 / 꺼진 몸통
//
// HasSink() 검사를 매크로 몸통에 두는 것이 핵심이다. 핸들러 안에서 검사하면
// 포맷과 힙 할당 비용을 치른 뒤에 버리게 된다.
// -----------------------------------------------------------------------------
#define ENGINE_LOG_IMPL(...)                                                   \
    do                                                                         \
    {                                                                          \
            ::engine::log::OnLogMessage(                                       \
                ::engine::log::FormatString(__VA_ARGS__));                    \
    } while (false)

#define ENGINE_SKIP_LOG_IMPL(...)                                              \
    do                                                                         \
    {                                                                          \
        if constexpr (false)                                                   \
        {                                                                      \
            ::engine::log::Unused(__VA_ARGS__);                                \
        }                                                                      \
    } while (false)

