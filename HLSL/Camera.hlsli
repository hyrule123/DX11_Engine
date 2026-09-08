#ifndef HLSL_CAMERA
#define HLSL_CAMERA
#include <Engine/HLSL/Core/CoreMinimal.hlsli>

struct alignas(16) CameraData
{
	matrix view_mat;
	matrix proj_mat;
};

#define REG_B_CAMERA			   REG_B_PER_PASS_0
#ifdef __HLSL
cbuffer CB_Camera : register(REG_B_CAMERA)
{
	CameraData g_camera;
}
#endif//__HLSL

#endif//HLSL_CAMERA