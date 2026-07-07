#include <Engine/HLSL/Debug/Debug.hlsli>

float4 main(VS_OUT inpos) : SV_TARGET
{
	return float4(g_debug_instance_data[inpos.instance_ID].color, 1.0f);
}