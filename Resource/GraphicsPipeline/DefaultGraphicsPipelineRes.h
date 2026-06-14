#pragma once

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