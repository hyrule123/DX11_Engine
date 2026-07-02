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

	struct StringHasher
	{
		using is_transparent = void;

		size_t operator()(std::string_view s) const noexcept
		{
			return std::hash<std::string_view>{}(s);
		}

		size_t operator()(const std::string& s) const noexcept
		{
			return std::hash<std::string_view>{}(s);
		}

		size_t operator()(const char* s) const noexcept
		{
			return std::hash<std::string_view>{}(s);
		}
	};

	template <typename T>
	using StringHashMap = std::unordered_map<std::string, T, StringHasher, std::equal_to<>>;
}