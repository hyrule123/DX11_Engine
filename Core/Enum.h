#pragma once

#include <Engine/Core/StdType.h>

namespace engine
{
	//enum wrapping을 위한 namespace
	namespace ShaderStage
	{
		enum Flag : uint8
		{
			kNone = 0,
			kVS = 1 << 0,
			kGS = 1 << 1,
			kPS = 1 << 2,
			kCS = 1 << 3,

			kAllGraphics = kVS | kGS | kPS
		};
		using Flags = uint8;
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
