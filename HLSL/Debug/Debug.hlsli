#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

#ifdef __HLSL

cbuffer wvp_cbuffer : register(SLOT_B_WVP)
{ 
	WVP wvp; 
};

struct VS_IN
{
	float3 pos : POSITION;
	uint instance_ID : SV_InstanceID;
};

struct VS_OUT
{
	float4 pos : SV_Position;
};

#endif // __HLSL


#endif//HLSL_DEBUG