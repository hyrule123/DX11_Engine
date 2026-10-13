#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#include <Engine/HLSL/Core/CoreMinimal.hlsli>
#include <Engine/HLSL/RenderPass.hlsli>
#include <Engine/HLSL/Camera.hlsli>

struct alignas(16) DebugDrawPerInstanceData
{
	matrix world_mat;
	float3 color;
	float left_time;
};

struct DebugVSInput
{
	float3 position SEMANTIC(POSITION);
	
#ifdef __HLSL
	uint instance_id : SV_InstanceID;
#endif //__HLSL
};

#ifdef __HLSL

struct VS_OUT
{
	float4 position : SV_Position;
	nointerpolation float3 color : COLOR;
};

#endif // __HLSL


#endif//HLSL_DEBUG