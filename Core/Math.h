#pragma once

#include <Engine/ThirdParty/SimpleMath.h>
#include <Engine/Core/StdType.h>

// Type aliases for compatibility with HLSL types
using float2 = engine::Vector2;
using float3 = engine::Vector3;
using float4 = engine::Vector4;
using matrix = engine::Matrix;
using uint = engine::uint32;

namespace engine
{
	constexpr float kEpsilon = 1e-6f;
}