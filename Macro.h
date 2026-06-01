#pragma once



#ifdef _DEBUG

#define ERROR_MESSAGE_W(_c_str) \
	do { MessageBoxW(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR); __debugbreak(); } while(false)
#define ERROR_MESSAGE_A(_c_str) \
	do { MessageBoxA(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR); __debugbreak(); } while(false)

#define DEBUG_MESSAGE_W(_c_str) \
	do { MessageBoxW(nullptr, _c_str, nullptr, MB_OK | MB_ICONINFORMATION); __debugbreak(); } while(false)
#define DEBUG_MESSAGE_A(_c_str) \
	do { MessageBoxW(nullptr, _c_str, nullptr, MB_OK | MB_ICONINFORMATION); __debugbreak(); } while(false)

#define DEBUG_LOG(_c_str) OutputDebugStringW(L ## _c_str)
#define DEBUG_LOG_A(_str) OutputDebugStringA(_str)
#define DEBUG_LOG_W(_wStr) OutputDebugStringW(_wStr)

#define DEBUG_BREAK ::__debugbreak()

#else //RELEASE

#define ERROR_MESSAGE_W(_c_str) MessageBoxW(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR)
#define ERROR_MESSAGE_A(_c_str) MessageBoxA(nullptr, _c_str, nullptr, MB_OK | MB_ICONERROR)

#define DEBUG_MESSAGE_W(_c_str) (void)0
#define DEBUG_MESSAGE_A(_c_str) (void)0

#define DEBUG_LOG(_c_str) (void)0
#define DEBUG_LOG_A(_str) (void)0
#define DEBUG_LOG_W(_wStr) (void)0

#define DEBUG_BREAK (void)0

#endif _DEBUG

#define NOTIFICATION_W(_c_str) MessageBoxW(nullptr, _c_str, L"Notification", MB_OK)
#define NOTIFICATION_A(_c_str) MessageBoxA(nullptr, _c_str, "Notification", MB_OK)

#define ERROR_MESSAGE(_literalString) ERROR_MESSAGE_W(L##_literalString)
#define NOTIFICATION(_literalString) NOTIFICATION_W(L##_literalString)
#define DEBUG_MESSAGE(_literalString) DEBUG_MESSAGE_W(L##_literalString)

#define SAFE_DELETE(_ptr) if (_ptr) { delete _ptr; _ptr = nullptr; }
#define SAFE_DELETE_ARRAY(_ptr_arr) if (_ptr_arr) { delete[] _ptr_arr; _ptr_arr = nullptr; }