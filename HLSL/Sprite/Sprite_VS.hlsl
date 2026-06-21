#include <Engine/HLSL/Sprite/Sprite.hlsli>

VS_OUT main(VS_IN indata)
{
	VS_OUT outdata;
	
	outdata.pos = mul(float4(indata.pos, 1.0f), obj.world_mat);
	outdata.pos = mul(outdata.pos, camera.view_mat);
	outdata.pos = mul(outdata.pos, camera.proj_mat);
	outdata.UV = indata.UV;
	
	return outdata;
}