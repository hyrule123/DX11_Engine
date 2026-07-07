#ifndef HLSL_TYPE
#define HLSL_TYPE

#include <Engine/HLSL/CppShared/Config.hlsli>

#define BOOL int
#define TRUE 1
#define FALSE 0

#ifdef __HLSL
#define alignas(_Num) 
#define SEMANTIC(_semantic) : _semantic;
#endif//__HLSL

#ifdef __cplusplus
#include <Engine/Core/Math.h>
#define SEMANTIC(_semantic) ;
#endif//__cplusplus

#endif//HLSL_TYPE