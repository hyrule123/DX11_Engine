#ifndef HLSL_DEBUG
#define HLSL_DEBUG

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

struct alignas(16)   DebugInstanceData
{
	matrix world_mat;
	float3 color;
	float padding_0;
};

struct DebugVSInput
{
	float3 position SEMANTIC(POSITION)
	
#ifdef __HLSL
	uint instance_ID : SV_InstanceID;
#endif //__HLSL
};

#ifdef __HLSL
#include <Engine/HLSL/CommonConstBuffer.hlsli>

StructuredBuffer<DebugInstanceData> g_debug_instance_data : register(SLOT_T_PER_INSTANCE);

struct VS_OUT
{
	float4 position : SV_Position;
	uint instance_ID : SV_InstanceID;
};

#endif // __HLSL


#endif//HLSL_DEBUG