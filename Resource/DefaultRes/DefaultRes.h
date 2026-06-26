#pragma once

namespace engine
{
	class DefaultRes
	{
		friend class ResourceManager;
	private:
		static void LoadDefaultResources();

		static void LoadDefaultRasterizerStates();
		static void LoadDefaultDepthStencilStates();
		static void LoadDefaultBlendStates();
		static void LoadDebugRenderObjects();
		static void LoadSpriteRenderObjects();


	private:
		~DefaultRes() = delete;
	};
}