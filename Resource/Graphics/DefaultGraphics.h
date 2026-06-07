#pragma once

#include <Engine/Common.h>

namespace engine
{
	class DefaultGraphics
	{
	public:
		static void CreateDefaultGraphicsResource();

	private:
		DefaultGraphics() = delete;
		~DefaultGraphics() = delete;
	};
}