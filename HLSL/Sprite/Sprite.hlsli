#ifndef HLSL_SPRITE
#define HLSL_SPRITE

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CommonConstBuffer.hlsli>
#include <Engine/HLSL/CommonSampler.hlsli>

Texture2DArray base_color : register(SLOT_T_BASE_COLOR);

struct VS_IN
{
	float3 pos : POSITION;
	float2 UV : TEXCOORD;
	uint instance_ID : SV_InstanceID;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float2 UV : TEXCOORD;
};

#endif//HLSL_SPRITE