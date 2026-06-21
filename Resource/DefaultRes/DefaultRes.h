#pragma once

namespace engine
{
	class DefaultRes
	{
		friend class ResourceManager;
	private:
		static void LoadDebugRenderObjects();
		static void LoadSpriteRenderObjects();

	private:
		~DefaultRes() = delete;
	};
}