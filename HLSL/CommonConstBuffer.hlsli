#ifndef HLSL_COMMON_CONST_BUFFER
#define HLSL_COMMON_CONST_BUFFER

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

#ifdef __HLSL
cbuffer cbuffer_per_pass : register(SLOT_B_PER_PASS)
{
	PerPass g_CB_camera;
}
#endif//__HLSL


#endif//HLSL_COMMON_CONST_BUFFER