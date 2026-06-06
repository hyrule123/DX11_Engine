#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(VS_IN inpos)
{
	VS_OUT outpos;
	outpos.pos = inpos.pos;
	
	return outpos;
}