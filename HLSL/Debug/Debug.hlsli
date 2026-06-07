#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#ifdef __HLSL

#include <Engine/HLSL/Common.hlsli>
struct VS_IN
{
	float4 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_Position;
};

#endif

#endif//HLSL_DEBUG