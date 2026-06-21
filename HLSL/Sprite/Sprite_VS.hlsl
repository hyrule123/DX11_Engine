#include <Engine/HLSL/Sprite/Sprite.hlsli>

VS_OUT main(VS_IN indata)
{
	VS_OUT outdata;
	
	outdata.pos = float4(indata.pos, 1.0f);
	outdata.UV = indata.UV;
	
	return outdata;
}