#pragma once

#include <Engine/Core/StdType.h>
#include <Engine/Core/Debug.h>

#define ENABLE_BITMASK_OPERATORS(EnumType)                                   \
    constexpr EnumType operator|(EnumType a, EnumType b) {                   \
        using U = std::underlying_type_t<EnumType>;                          \
        return static_cast<EnumType>(static_cast<U>(a) | static_cast<U>(b)); \
    }                                                                        \
    constexpr EnumType& operator|=(EnumType& a, EnumType b) {                \
        return a = a | b;                                                    \
    }                                                                        \
    constexpr bool HasFlag(EnumType v, EnumType f) {                         \
        using U = std::underlying_type_t<EnumType>;                          \
        return (static_cast<U>(v) & static_cast<U>(f)) != U{0};              \
    }

namespace engine
{
	// Enum To Flags
	template <typename EnumT, typename StorageT>
	class EnumFlags
	{
	public:
		static_assert(std::is_enum_v<EnumT>);
		static_assert((size_t)EnumT::kCount <= sizeof(StorageT) * 8,
			"열거자 개수가 저장 타입 비트 폭을 초과했습니다.");

		constexpr EnumFlags() = default;
		constexpr explicit EnumFlags(EnumT e) : bits_(ToBit(e)) {}

		constexpr bool Has(EnumT e) const { return (bits_ & ToBit(e)) != 0; }
		constexpr void Set(EnumT e) { bits_ |= ToBit(e); }
		constexpr void Clear(EnumT e) { bits_ &= ~ToBit(e); }

		constexpr bool None() const { return bits_ == 0; }
		constexpr bool Any()  const { return bits_ != 0; }

		constexpr EnumFlags& operator|=(EnumFlags o) { bits_ |= o.bits_; return *this; }
		constexpr EnumFlags operator|(EnumFlags o) const { return EnumFlags(bits_ | o.bits_); }
		constexpr bool operator==(const EnumFlags&) const = default;

		static constexpr EnumFlags All() { return EnumFlags(StorageT((StorageT{ 1 } << (StorageT)EnumT::kCount) - 1)); }

	private:
		static constexpr StorageT ToBit(EnumT e) { return StorageT{ 1 } << (StorageT)e; }

		constexpr explicit EnumFlags(StorageT raw) : bits_(raw) {}
		StorageT bits_ = 0;
	};

	//enum wrapping을 위한 namespace
	namespace ShaderStage
	{
		enum class Enum : uint8
		{
			Vertex = 0,
			Geometry,
			Pixel,
			Compute,
			kEND
		};

		enum class Flags : uint8
		{
			None = 0,
			Vertex = 1 << static_cast<uint8>(Enum::Vertex),
			Geometry = 1 << static_cast<uint8>(Enum::Geometry),
			Pixel = 1 << static_cast<uint8>(Enum::Pixel),
			Compute = 1 << static_cast<uint8>(Enum::Compute),
			AllGraphics = Vertex | Geometry | Pixel,
			All = Vertex | Geometry | Pixel | Compute
		};
		ENABLE_BITMASK_OPERATORS(Flags);

		constexpr Flags EnumToFlag(Enum stage)
		{
			ASSERT(stage < Enum::kEND);
			return static_cast<Flags>(1 << static_cast<uint8>(stage));
		}
	}

	enum class SubscribeType : uint8
	{
		kCollision,
		kTransformDirty,
		kLayerChanged,
		kEND
	};
}
