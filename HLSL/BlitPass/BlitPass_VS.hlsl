#include <Engine/HLSL/BlitPass/BlitPass.hlsli>

PresentVSOutput main(PresentVSInput input)
{
	PresentVSOutput output;
	output.position = float4(input.position, 1.0);
	output.uv = input.uv;
	return output;
}