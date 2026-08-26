#pragma once

#include <Engine/Core/StdType.h>

namespace engine
{
	//enum wrapping을 위한 namespace
	namespace ShaderStage
	{
		enum Flag : uint32
		{
			kNone = 0,
			kVS = 1 << 0,
			kGS = 1 << 1,
			kPS = 1 << 2,
			kCS = 1 << 3,

			kAllGraphics = kVS | kGS | kPS
		};
		using Flags = uint32;
	}

	enum class RenderPassOrder : uint32
	{
		kForwardOpaque = 0u,
		kForwardAlphaTested,
		kForwardTransparent,
		kUI,
		kPresent,
		kEND
	};

	enum class SubscribeType : uint32
	{
		kCollision,
		kTransformDirty,
		kLayerChanged,
		kEND
	};
}
