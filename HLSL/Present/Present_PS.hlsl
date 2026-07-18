#include <Engine/HLSL/Present/Present.hlsli>

#include <Engine/HLSL/CommonSampler.hlsli>

float4 main(PresentVSOutput input) : SV_TARGET
{
	float4 color = src_rendertarget.Sample(g_point_clamp_samper, input.UV);
	
	return color;
}