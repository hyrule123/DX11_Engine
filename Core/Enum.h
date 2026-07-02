#pragma once

#include <Engine/Core/StdType.h>

namespace engine
{
	//enum wrapping을 위한 namespace
	namespace ShaderStage
	{
		enum Flag : uint32
		{
			kVS = 1 << 0,
			kGS = 1 << 1,
			kPS = 1 << 2,
			kCS = 1 << 3,

			kAllGraphics = kVS | kGS | kPS
		};
		using Flags = uint32;
	}

	namespace RenderPassType
	{
		enum class Order : uint32
		{
			kForwardOpaque = 0u,
			kForwardAlphaTested,
			kForwardTransparent,
			kUI,
			kEND
		};

		enum Flag : uint32
		{
			kForwardOpaque = 1 << (uint32)Order::kForwardOpaque,
			kForwardAlphaTested = 1 << (uint32)Order::kForwardAlphaTested,
			kForwardTransparent = 1 << (uint32)Order::kForwardTransparent,
			kUI = 1 << (uint32)Order::kUI,
		};

		using Flags = uint32;
	}
}
