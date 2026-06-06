#pragma once

#include <Engine/Type.h>

namespace engine
{
	struct VertexDebug
	{
		float4 position;
		float4 color;
	};

	struct Vertex2D {
		float4 position;
		float2 UV;
	};
}