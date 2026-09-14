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

#define ENGINE_CHECK_MSG_IMPL(_expression, ...)                      \
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

#define ENGINE_ASSERT_MSG_IMPL(_expression, ...)                     \
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

#define ENGINE_SKIP_CHECK_MSG_IMPL(_expression, ...)                 \
  do                                                                 \
  {                                                                  \
    if constexpr (false)                                             \
    {                                                                \
      ::engine::detail::AssertUnused(!(_expression), __VA_ARGS__);   \
    }                                                                \
  } while (false)