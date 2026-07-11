#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>

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

	union RenderKey
	{
		struct
		{
			uint32 material_id;
			uint32 mesh_id;
		};
		uint64 key = {};

		auto operator<=>(const RenderKey& other) const {
			return key <=> other.key;
		}
		auto operator==(const RenderKey& other) const {
			return key == other.key;
		}
	};

	struct RenderKeyHasher
	{
		using is_transparent = void;
		size_t operator()(const RenderKey& key) const noexcept
		{
			return std::hash<uint64>{}(key.key);
		}
	};

	struct DataBlock
	{
		void* ptr = {};
		size_t size = {};
	};
}