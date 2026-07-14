#include <Engine/HLSL/Sprite/Sprite.hlsli>

VS_OUT main(SpriteVSInput indata)
{
	VS_OUT outdata;
	
	outdata.position = mul(float4(indata.position, 1.0f), g_sprite_instance_data[indata.instance_ID].world_mat);
	outdata.position = mul(outdata.position, g_CB_camera.view_mat);
	outdata.position = mul(outdata.position, g_CB_camera.proj_mat);
	
	if (g_sprite_instance_data[indata.instance_ID].flip_left_right)
	{
		outdata.UV.x = 1.0f - indata.UV.x;
	}
	else
	{
		outdata.UV.x = indata.UV.x;
	}
	outdata.UV.y = indata.UV.y;
	outdata.instance_ID = indata.instance_ID;
	
	return outdata;
}