#include <Engine/HLSL/BlitPass/BlitPass.hlsli>

BlitVSOutput main(BlitVSInput input)
{
	BlitVSOutput output;
	output.position = float4(input.position, 1.0);
	output.uv = input.uv;
	return output;
}