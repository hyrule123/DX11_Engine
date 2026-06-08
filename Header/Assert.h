#pragma once
#include <cassert>

#define ASSERT(_expression) assert(_expression)

#ifdef _DEBUG

#define ASSERT_RELEASE(_expression) assert(_expression)

#define ASSERT_MESSAGE(_expression, _c_str) \
	do \
	{ \
		if (!(_expression)) \
		{ _wassert(L ## #_expression##"\n\n"##_c_str , _CRT_WIDE(__FILE__), (unsigned)(__LINE__)); } \
	} \
	while (false)

#define ASSERT_RELEASE_MESSAGE(_expression, _c_str) ASSERT_MESSAGE(_expression, _c_str)

#else //RELEASE

#define ASSERT (void)0

#define ASSERT_MESSAGE (void)0

#define ASSERT_RELEASE (_expression) \
	do \
	{ \
		if(!(_expression)) \
		{ \
		MessageBoxW(nullptr, L## #_expression, L"Assertion Failed!", MB_OK | MB_ICONERROR);\
		std::abort();\
		} \
	}\
	while(false)

#define ASSERT_RELEASE_MESSAGE(_expression, _c_str) \
	do \
	{ \
		if(!(_expression)) \
		{ \
		MessageBoxW(nullptr, L## #_expression##"\n\n"##_c_str, L"Assertion Failed!", MB_OK | MB_ICONERROR);\
		std::abort();\
		} \
	}\
	while(false)

#define ERROR_MESSAGE_W(_c_str) MessageBoxW(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR)
#define ERROR_MESSAGE_A(_c_str) MessageBoxA(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR)

#endif _DEBUG
