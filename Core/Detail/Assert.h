// Assert.h
#pragma once

#include <format>
#include <string>
#include <string_view>

#include <intrin.h>  // __debugbreak

#define ENGINE_DEBUG_BREAK __debugbreak()

namespace engine::detail
{
    // 공통 구현. 디버거가 없으면 대화상자 후 abort() 로 끝난다.
    void ReportAssertFailure(const char* file, int line, const char* expr,
        std::string_view message);

    // __debugbreak() 는 실패 지점 유지를 위해 호출부(매크로)에 작성됨.
    inline void OnAssertFailed(const char* file, int line, const char* expr)
    {
        ReportAssertFailure(file, line, expr, {});
    }

    // 리터럴이 확정된 Args... 로 암시적 변환되면서
    // consteval 생성자가 포맷/타입 정합성을 컴파일 타임에 검사한다.
    template <typename... Args>
    void OnAssertFailed(const char* file, int line, const char* expr,
        std::format_string<Args...> fmt, Args&&... args)
    {
        ReportAssertFailure(file, line, expr,
            std::format(fmt, std::forward<Args>(args)...));
    }

    // 꺼진 빌드용. 호출되지 않지만 의미 검사는 받는다.
    constexpr void AssertUnused(bool) noexcept {}
    template <typename... Args>
    constexpr void AssertUnused(bool, std::format_string<Args...>,
        Args&&...) noexcept
    {}


// --- ENSURE: 실패해도 중단하지 않는다 ---
// 조건값을 그대로 반환하므로 if (!ENSURE(x)) { ... } 형태로 복구 경로를 쓴다.
// 실패 시 보고하고, 디버거가 붙어 있으면 break 한다. 없으면 계속 진행.
// abort() 하지 않는 것이 ASSERT/CHECK 와의 계약 차이.
// __debugbreak() 가 이 함수 안에서 일어나므로 실패 지점이 콜스택
// 한 칸 위로 밀린다 — 식으로 써야 해서 감수한 부분.
    void ReportEnsureFailure(const char* file, int line, const char* expr,
        std::string_view message);

    inline bool OnEnsureFailed(bool condition, const char* file, int line,
        const char* expr)
    {
        if (!condition)
        {
            ReportEnsureFailure(file, line, expr, {});
        }

        return condition;
    }

    template <typename... Args>
    bool OnEnsureFailed(bool condition, const char* file, int line,
        const char* expr, std::format_string<Args...> fmt,
        Args&&... args)
    {
        if (!condition)
        {
            ReportEnsureFailure(file, line, expr,
                std::format(fmt, std::forward<Args>(args)...));
        }

        return condition;
    }

}  // namespace engine::detail

#define ENGINE_CHECK_IMPL(_expression)								 \
  do                                                                 \
  {                                                                  \
    if (!(_expression))                                              \
    {                                                                \
      ::engine::detail::OnAssertFailed(__FILE__, __LINE__,           \
                                      #_expression);                 \
      ENGINE_DEBUG_BREAK;                                            \
    }                                                                \
  } while (false)

#define ENGINE_CHECK_F_IMPL(_expression, ...)                      \
    do                                                               \
    {                                                                \
    if (!(_expression))                                              \
    {                                                                \
    ::engine::detail::OnAssertFailed(__FILE__, __LINE__,             \
	    #_expression, __VA_ARGS__);                                  \
	    ENGINE_DEBUG_BREAK;                                          \
    }                                                                \
    } while (false)

#define ENGINE_ASSERT_IMPL(_expression)                              \
  do                                                                 \
  {                                                                  \
    if (!(_expression))                                              \
    {                                                                \
      ::engine::detail::OnAssertFailed(__FILE__, __LINE__,           \
                                       #_expression);                \
      ENGINE_DEBUG_BREAK;                                            \
    }                                                                \
  } while (false)

#define ENGINE_ASSERT_F_IMPL(_expression, ...)                     \
  do                                                                 \
  {                                                                  \
    if (!(_expression))                                              \
    {                                                                \
      ::engine::detail::OnAssertFailed(__FILE__, __LINE__,           \
                                       #_expression, __VA_ARGS__);   \
      ENGINE_DEBUG_BREAK;                                            \
    }                                                                \
  } while (false)

// if constexpr (false) 의 폐기된 분기는 코드 생성이 일어나지 않는다.
// 의미 검사만 받으므로 런타임 비용은 0이면서 조건식·포맷 인자가
// 컴파일 검증을 받고 미사용 변수 경고도 방지된다.
#define ENGINE_SKIP_CHECK_IMPL(_expression)                          \
  do                                                                 \
  {                                                                  \
    if constexpr (false)                                             \
    {                                                                \
      ::engine::detail::AssertUnused(!(_expression));                \
    }                                                                \
  } while (false)

#define ENGINE_SKIP_CHECK_F_IMPL(_expression, ...)                 \
  do                                                                 \
  {                                                                  \
    if constexpr (false)                                             \
    {                                                                \
      ::engine::detail::AssertUnused(!(_expression), __VA_ARGS__);   \
    }                                                                \
  } while (false)

#define ENGINE_ENSURE_IMPL(_expression)                              \
  (::engine::detail::OnEnsureFailed(!!(_expression), __FILE__,       \
                                    __LINE__, #_expression))

#define ENGINE_ENSURE_F_IMPL(_expression, ...)                     \
  (::engine::detail::OnEnsureFailed(!!(_expression), __FILE__,       \
                                    __LINE__, #_expression,          \
                                    __VA_ARGS__))