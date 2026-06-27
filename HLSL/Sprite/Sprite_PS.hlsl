#include <Engine/HLSL/Sprite/Sprite.hlsli>

float4 main(VS_OUT indata) : SV_TARGET
{
	float4 outcolor = base_color.Sample(g_point_clamp_samper, float3(indata.UV, g_CB_obj.test_frame));
	return outcolor;
}