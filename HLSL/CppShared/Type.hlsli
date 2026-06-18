#ifndef HLSL_COMMON
#define HLSL_COMMON

#define BOOL int
#define TRUE 1
#define FALSE 0

#ifdef __HLSL
#define alignas(_Num)
#endif//__HLSL

#endif//HLSL_COMMON