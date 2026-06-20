#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(VS_IN inpos)
{
	VS_OUT outpos;
	
	outpos.pos = mul(float4(inpos.pos, 1.0f), world_mat);
	
	outpos.pos = mul(outpos.pos, camera.view_mat);
	outpos.pos = mul(outpos.pos, camera.proj_mat);
	
	return outpos;
}