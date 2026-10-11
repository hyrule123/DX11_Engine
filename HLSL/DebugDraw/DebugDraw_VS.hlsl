#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

StructuredBuffer<DebugDrawPerInstanceData> g_debug_instance_data : register(REG_T_INSTANCE_BUFFER);

VS_OUT main(DebugVSInput inpos)
{
	VS_OUT outpos;
	
	outpos.position = mul(float4(inpos.position, 1.0f), g_debug_instance_data[inpos.instance_id].world_mat);
	outpos.position.z = 0.0f;
	
	outpos.position = mul(outpos.position, g_camera.view_mat);
	outpos.position = mul(outpos.position, g_camera.proj_mat);
	
	outpos.color = g_debug_instance_data[inpos.instance_id].color;
	
	outpos.instance_id = inpos.instance_id;
	
	return outpos;
}