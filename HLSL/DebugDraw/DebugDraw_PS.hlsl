#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

float4 main(VS_OUT inpos) : SV_TARGET
{
	return float4(inpos.color, 1.0f);
}