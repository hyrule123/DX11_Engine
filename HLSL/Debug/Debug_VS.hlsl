#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(VS_IN inpos)
{
	VS_OUT outpos;
	
	outpos.pos = mul(float4(inpos.pos, 1.0f), g_CB_obj.world_mat);
	
	outpos.pos = mul(outpos.pos, g_CB_camera.view_mat);
	outpos.pos = mul(outpos.pos, g_CB_camera.proj_mat);
	
	return outpos;
}