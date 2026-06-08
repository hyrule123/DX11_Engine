#pragma once

#include <cstdint>
#include <memory>
#include <wrl.h>
#include <Engine/Util/SimpleMath.h>

namespace engine
{
	/* Standard integer types */
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	// Type aliases for compatibility with HLSL types
	using float2 = Vector2;
	using float3 = Vector3;
	using float4 = Vector4;
	using MATRIX = Matrix;

	/* Smart pointer types */
	template <typename T>
	using u_ptr = std::unique_ptr<T>;
	template <typename T>
	using s_ptr = std::shared_ptr<T>;
	template <typename T>
	using w_ptr = std::weak_ptr<T>;

	using Microsoft::WRL::ComPtr;
}