#include <Engine/HLSL/BlitPass/BlitPass.hlsli>

#include <Engine/HLSL/CommonSampler.hlsli>

Texture2D g_src_rendertarget : register(REG_T_SRC_RENDERTARGET);

float4 main(BlitVSOutput input) : SV_TARGET
{
	float4 color = g_src_rendertarget.Sample(g_point_clamp_sampler, input.uv);
	return color;
}