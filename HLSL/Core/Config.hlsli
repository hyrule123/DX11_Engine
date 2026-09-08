#ifndef HLSL_CONFIG
#define HLSL_CONFIG

#ifndef __cplusplus
#ifndef __HLSL
#define __HLSL
#endif//__HLSL
#endif//__cplusplus

#ifdef __HLSL
#pragma pack_matrix(row_major)
#endif//__HLSL

#define MAX_TEXTURE_COUNT 8

#endif//HLSL_CONFIG