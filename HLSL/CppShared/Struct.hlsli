#ifndef HLSL_STRUCT
#define HLSL_STRUCT

#include <Engine/HLSL/CppShared/Type.hlsli>
#include <Engine/HLSL/CppShared/Register.hlsli>

#ifdef __cplusplus
#include <Engine/Core/Math.h>
namespace engine
{
#endif//__cplusplus

struct alignas(16) CameraData
{
	matrix view_mat, proj_mat;
};

#ifdef __cplusplus
};
#endif//__cplusplus

#endif//HLSL_STRUCT