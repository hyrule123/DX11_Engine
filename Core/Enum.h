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

	enum class BufferUsage
	{
		kNone,
		kImmutable, // CPU 데이터 수정 불가. GPU 읽기만 가능
		kDefault,   // CPU 데이터 수정 불가. GPU 읽기/쓰기 가능
		kDynamic,   // CPU 데이터 수정 가능. GPU 읽기만 가능
		kStaging    // CPU 데이터 수정 가능. GPU 읽기/쓰기 불가
	};

	namespace BufferBind
	{
		enum Flag : uint32
		{
			kBindNone = 0,
			kBindShaderResource = 1 << 0,
			kBindUnorderedAccess = 1 << 1,
			kBindRenderTarget = 1 << 2,
			kBindDepthStencil = 1 << 3
			//...
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
