#pragma once

#include <Engine/Core/Common.h>

namespace engine
{
	class DefaultGraphicsPipelineRes
	{
		friend class ResourceManager;

	private:
		static void Load();

		DefaultGraphicsPipelineRes() = delete;
		~DefaultGraphicsPipelineRes() = delete;
	};
}