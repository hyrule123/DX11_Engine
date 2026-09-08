#ifndef HLSL_SPRITE
#define HLSL_SPRITE

#include <Engine/HLSL/Core/CoreMinimal.hlsli>
#include <Engine/HLSL/RenderPass.hlsli>
#include <Engine/HLSL/Camera.hlsli>

#define REG_T_BASE_COLOR REG_T_PER_MATERIAL_0

struct SpriteInstanceData
{
	matrix world_mat;
	uint sprite_frame_idx;
	BOOL flip_left_right;
	float2 padding_0;
};

struct SpriteVSInput
{
	float3 position SEMANTIC(POSITION);
	float2 uv SEMANTIC(TEXCOORD);
	
	#ifdef __HLSL
	uint instance_ID :SV_InstanceID;
	#endif //__HLSL
};

#ifdef __HLSL
#include <Engine/HLSL/CommonSampler.hlsli>

Texture2DArray g_base_color : register(REG_T_BASE_COLOR);
StructuredBuffer<SpriteInstanceData> g_sprite_instance_data : register(REG_T_INSTANCE_BUFFER);

struct VS_OUT
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD;
	uint instance_ID : SV_InstanceID;
};
#endif//__HLSL

#endif//HLSL_SPRITE