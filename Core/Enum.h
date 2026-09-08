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
			ASSERT_RELEASE(stage < Enum::kEND);
			return static_cast<Flags>(1 << static_cast<uint8>(stage));
		}

		constexpr Enum FlagToEnum(Flags stage_flag)
		{
			for (uint8 i = 0; i < static_cast<uint8>(Enum::kEND); ++i)
			{
				Enum stage = static_cast<Enum>(i);
				if (HasFlag(stage_flag, EnumToFlag(stage)))
				{
					return stage;
				}
			}
			ASSERT_RELEASE(false && "Invalid ShaderStage::Flags");
			return Enum::kEND;
		}
	}

	enum class RenderPassOrder : uint8
	{
		kForwardOpaque = 0u,
		kForwardAlphaTested,
		kForwardTransparent,
		kUI,
		kPresent,
		kEND
	};

	enum class SubscribeType : uint8
	{
		kCollision,
		kTransformDirty,
		kLayerChanged,
		kEND
	};
}
