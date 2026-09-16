#pragma once
#define NOMINMAX

#include <Engine/ThirdParty/DirectXTK/SimpleMath.h>
#include <Engine/Core/CoreTypes.h>

#include <limits>

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
	constexpr float kFloatMax = std::numeric_limits<float>::max();

	//참고: float min = '양수 최소값'
	constexpr float kFloatLowest = std::numeric_limits<float>::lowest();

	constexpr float2 kFloat2Max = float2(kFloatMax, kFloatMax);
	constexpr float2 kFloat2Lowest = float2(kFloatLowest, kFloatLowest);

	constexpr float3 kFloat3Max = float3(kFloatMax, kFloatMax, kFloatMax);
	constexpr float3 kFloat3Lowest = float3(kFloatLowest, kFloatLowest, kFloatLowest);
	
}