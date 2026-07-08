#ifndef HLSL_COMMON_SAMPLER
#define HLSL_COMMON_SAMPLER

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>

#ifdef __HLSL
SamplerState g_point_clamp_samper : register(SLOT_S_POINT_CLAMP);
#endif//__HLSL

#endif//HLSL_COMMON_SAMPLER