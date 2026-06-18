#ifndef HLSL_STRUCT
#define HLSL_STRUCT

#include <Engine/HLSL/CppShared/Type.hlsli>
#include <Engine/HLSL/CppShared/Register.hlsli>

struct WVP
{
	matrix world, view, pos;
};

#endif//HLSL_STRUCT