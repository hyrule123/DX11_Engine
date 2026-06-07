#include <Engine/HLSL/Debug/Debug.hlsli>

VS_OUT main(VS_IN inpos)
{
	VS_OUT outpos;
	
	const MATRIX mymat =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	
	outpos.pos = mul(inpos.pos, mymat);
	
	return outpos;
}