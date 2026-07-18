#ifndef HLSL_PRESENT
#define HLSL_PRESENT

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>

struct PresentVSInput
{
	float3 position SEMANTIC(POSITION)
	float2 UV SEMANTIC(TEXCOORD)
};

#ifdef __HLSL

Texture2D src_rendertarget : register(SLOT_T_BASE_COLOR);

struct PresentVSOutput
{
	float4 position : SV_Position;
	float2 UV : TEXCOORD;
};

#endif //__HLSL

#endif//HLSL_PRESENT