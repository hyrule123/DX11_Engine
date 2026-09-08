#include <Engine/HLSL/PresentPass/PresentPass.hlsli>

#include <Engine/HLSL/CommonSampler.hlsli>

float4 main(PresentVSOutput input) : SV_TARGET
{
	float4 color = g_src_rendertarget.Sample(g_point_clamp_samper, input.uv);
	return color;
}