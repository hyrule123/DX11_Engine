#include <Engine/HLSL/Sprite/Sprite.hlsli>

float4 main(VS_OUT indata) : SV_TARGET
{
	float4 outcolor = g_base_color.Sample(g_point_clamp_samper, float3(indata.uv, g_sprite_instance_data[indata.instance_ID].sprite_frame_idx));
	
	if (outcolor.a == 0.0f) { discard; }
	
	return outcolor;
}

