#pragma once
#include <cstdint>

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

	struct int32_2 { int32 x, y; };

	//enum wrapping을 위한 namespace
	namespace ShaderStage
	{
		enum ShaderStageBitMask : uint32
		{
			kVS = 1 << 0,
			kGS = 1 << 1,
			kPS = 1 << 2,
			kCS = 1 << 3,

			kAllGraphics = kVS | kGS | kPS
		};
	}
	using ShaderStageFlag = uint32;
}