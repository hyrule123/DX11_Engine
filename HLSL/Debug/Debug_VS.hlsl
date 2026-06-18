#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(VS_IN inpos)
{
	VS_OUT outpos;
	
	outpos.pos = mul(float4(inpos.pos, 1.0f), wvp.world);
	
	return outpos;
}