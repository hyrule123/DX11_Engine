#include <Engine/HLSL/Present/Present.hlsli>

PresentVSOutput main(PresentVSInput input)
{
	PresentVSOutput output;
	output.position = float4(input.position, 1.0);
	output.UV = input.UV;
	return output;
}