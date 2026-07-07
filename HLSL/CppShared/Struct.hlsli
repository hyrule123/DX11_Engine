#ifndef HLSL_STRUCT
#define HLSL_STRUCT

#include <Engine/HLSL/CppShared/Type.hlsli>
#include <Engine/HLSL/CppShared/Register.hlsli>

struct alignas(16) PerPass
{
	matrix view_mat;
	matrix proj_mat;
};

#endif//HLSL_STRUCT