#include <Engine/HLSL/Sprite/Sprite.hlsli>

VS_OUT main(VS_IN indata)
{
	VS_OUT outdata;
	
	outdata.pos = mul(float4(indata.pos, 1.0f), g_CB_obj.world_mat);
	outdata.pos = mul(outdata.pos, g_CB_camera.view_mat);
	outdata.pos = mul(outdata.pos, g_CB_camera.proj_mat);
	outdata.UV = indata.UV;
	
	return outdata;
}