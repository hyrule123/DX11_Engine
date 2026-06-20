#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

#ifdef __HLSL

cbuffer cbuffer_camera : register(SLOT_B_CAMERA)
{ 
	CameraData camera; 
}

cbuffer cbuffer_world : register(SLOT_B_WORLD)
{
	matrix world_mat;
}

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