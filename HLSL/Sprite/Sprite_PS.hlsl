#include <Engine/HLSL/Sprite/Sprite.hlsli>

float4 main(VS_OUT indata) : SV_TARGET
{
	float4 outcolor = base_color.Sample(default_sampler, indata.UV);
	return outcolor;
}