#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(DebugVSInput inpos)
{
	VS_OUT outpos;
	
	outpos.position = mul(float4(inpos.position, 1.0f), g_debug_instance_data[inpos.instance_ID].world_mat);
	
	outpos.position = mul(outpos.position, g_CB_camera.view_mat);
	outpos.position = mul(outpos.position, g_CB_camera.proj_mat);
	
	outpos.instance_ID = inpos.instance_ID;
	
	return outpos;
}