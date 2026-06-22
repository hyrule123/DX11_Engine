#ifndef HLSL_COMMON_CONST_BUFFER
#define HLSL_COMMON_CONST_BUFFER

#include <Engine/HLSL/CppShared/CoreMinimal.hlsli>
#include <Engine/HLSL/CppShared/Struct.hlsli>

//!!!!!!!!!!!! NO CPP INCLUDE !!!!!!!!!!!!!

cbuffer cbuffer_per_object : register(SLOT_B_PER_OBJECT)
{
	PerObj g_CB_obj;
}

cbuffer cbuffer_per_pass : register(SLOT_B_PER_PASS)
{
	PerPass g_CB_camera;
}



#endif//HLSL_COMMON_CONST_BUFFER