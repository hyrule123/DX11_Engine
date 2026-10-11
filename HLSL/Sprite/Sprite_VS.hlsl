#include <Engine/HLSL/Sprite/Sprite.hlsli>

VS_OUT main(SpriteVSInput indata)
{
	VS_OUT outdata;
	
	outdata.position = mul(float4(indata.position, 1.0f), g_sprite_instance_data[indata.instance_id].world_mat);
	outdata.position = mul(outdata.position, g_camera.view_mat);
	outdata.position = mul(outdata.position, g_camera.proj_mat);
	
	if (g_sprite_instance_data[indata.instance_id].flip_left_right)
	{
		outdata.uv.x = 1.0f - indata.uv.x;
	}
	else
	{
		outdata.uv.x = indata.uv.x;
	}
	outdata.uv.y = indata.uv.y;
	outdata.instance_id = indata.instance_id;
	
	return outdata;
}