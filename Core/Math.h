#pragma once
#define NOMINMAX

#include <Engine/ThirdParty/DirectXTK/SimpleMath.h>
#include <Engine/Core/StdType.h>

// Type aliases for compatibility with HLSL types
using float2 = engine::Vector2;
using float3 = engine::Vector3;
using float4 = engine::Vector4;
using matrix = engine::Matrix;
using uint = engine::uint32;
using uint2 = engine::uint32_2;
using int2 = engine::uint32_2;

namespace engine
{
	constexpr float kEpsilon = 1e-6f;
}