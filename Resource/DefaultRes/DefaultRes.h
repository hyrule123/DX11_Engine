#pragma once

namespace engine
{
	class DefaultRes
	{
		friend class ResourceManager;
	private:
		static void LoadDebugRenderObjects();

	private:
		~DefaultRes() = delete;
	};
}