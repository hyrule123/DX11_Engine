#pragma once

#include <Engine/Header/Common.h>

namespace engine
{
	class DefaultGraphicsRes
	{
		friend class ResourceManager;

	private:
		static void Load();

		DefaultGraphicsRes() = delete;
		~DefaultGraphicsRes() = delete;
	};
}