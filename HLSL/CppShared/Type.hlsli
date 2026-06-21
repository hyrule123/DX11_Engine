#ifndef HLSL_TYPE
#define HLSL_TYPE

#include <Engine/HLSL/CppShared/Config.hlsli>

#define BOOL int
#define TRUE 1
#define FALSE 0

#ifdef __HLSL
#define alignas(_Num) 
#endif//__HLSL

#endif//HLSL_TYPE