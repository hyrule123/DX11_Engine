#ifndef HLSL_SPRITE
#define HLSL_SPRITE

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CommonConstBuffer.hlsli>

struct VS_IN
{
	float3 pos : POSITION;
	float2 UV : TEXCOORD;
	uint instance_ID : SV_InstanceID;
};

struct VS_OUT
{
	float2 UV : TEXCOORD;
	float4 pos : SV_Position;
};

#endif//HLSL_SPRITE