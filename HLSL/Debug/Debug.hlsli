#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

#include <Engine/HLSL/CommonConstBuffer.hlsli>

#ifdef __HLSL



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