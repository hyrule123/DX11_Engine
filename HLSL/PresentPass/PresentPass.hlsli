#ifndef HLSL_PRESENT_PASS
#define HLSL_PRESENT_PASS

#include <Engine/HLSL/Core/CoreMinimal.hlsli>
#include <Engine/HLSL/RenderPass.hlsli>

#define REG_T_SRC_RENDERTARGET REG_T_PER_PASS_0

struct PresentVSInput
{
	float3 position SEMANTIC(POSITION);
	float2 uv SEMANTIC(TEXCOORD);
};

#ifdef __HLSL

Texture2D g_src_rendertarget : register(REG_T_SRC_RENDERTARGET);

struct PresentVSOutput
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD;
};

#endif //__HLSL

#endif//HLSL_PRESENT_PASS