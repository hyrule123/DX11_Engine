#pragma once
#include <Engine/Core/Detail/Assert.h>
#include <Engine/Core/Detail/EngineLog.h>


#ifndef ENGINE_ASSERT_ENABLED
#ifdef NDEBUG
#define ENGINE_ASSERT_ENABLED 0
#else
#define ENGINE_ASSERT_ENABLED 1
#endif //NDEBUG
#endif //ENGINE_ASSERT_ENABLED

#pragma region ASSERTION API
#define CHECK(_expression) ENGINE_CHECK_IMPL(_expression)
#define CHECK_F(_expression, ...) ENGINE_CHECK_MSG_IMPL(_expression, __VA_ARGS__)

#if ENGINE_ASSERT_ENABLED

#define ASSERT(_expression) ENGINE_ASSERT_IMPL(_expression)
#define ASSERT_F(_expression, ...) ENGINE_ASSERT_MSG_IMPL(_expression, __VA_ARGS__)

#else  // ENGINE_ASSERT_ENABLED

#define ASSERT(_expression) ENGINE_SKIP_CHECK_IMPL(_expression)
#define ASSERT_F(_expression, ...) ENGINE_SKIP_CHECK_MSG_IMPL(_expression, __VA_ARGS__)

#endif  // ENGINE_ASSERT_ENABLED

// -----------------------------------------------------------------------------
// 게이트
//   ENGINE_LOG_ENABLED: DEBUG_LOG 전용 스위치.
//   _DEBUG는 MSVC 전용이라 NDEBUG에서 유도한다 (ASSERT 쪽과 동일한 방침).
//   LOG / ERR_MSG는 이 게이트 밖에 있다.
// -----------------------------------------------------------------------------
#if !defined(ENGINE_LOG_ENABLED)
#if defined(NDEBUG)
#define ENGINE_LOG_ENABLED 0
#else
#define ENGINE_LOG_ENABLED 1
#endif
#endif

// 출력단이 붙어 있으면 로깅. 빌드 구성과 무관하게 항상 존재한다.
#define LOG(...) ENGINE_LOG_IMPL(__VA_ARGS__)

// Debug 빌드에서만 LOG. Release에서는 몸통이 사라진다.
#if ENGINE_LOG_ENABLED
#define DEBUG_LOG(...) ENGINE_LOG_IMPL(__VA_ARGS__)
#else
#define DEBUG_LOG(...) ENGINE_SKIP_LOG_IMPL(__VA_ARGS__)
#endif

// 무조건 MessageBox 표시. 억제 없음, 매번 표시.
// 확인 후 실행은 계속된다 (중단하지 않는다).
// 호출 시점 판단은 개발자에게 맡긴다 — 아래 주의사항 참고.
#define ERR_MSG(...)                                                           \
    do                                                                         \
    {                                                                          \
        if (::engine::log::OnErrorMessage(                                     \
                __FILE__, __LINE__,                                            \
                ::engine::log::FormatString(__VA_ARGS__)))                    \
        {                                                                      \
            ENGINE_DEBUG_BREAK;                                                \
        }                                                                      \
    } while (false)

#define ERR_MSG_HRESULT(_hresult)                                              \
    do                                                                         \
    {                                                                          \
        if (::engine::log::HResultErrorMessage(__FILE__, __LINE__, _hresult)) \
        {                                                                      \
            ENGINE_DEBUG_BREAK;                                                \
        }                                                                      \
    } while (false)                             