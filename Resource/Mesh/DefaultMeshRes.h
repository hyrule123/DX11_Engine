#pragma once



namespace engine
{
	class Resource;

	class DefaultMeshRes
	{
		friend class ResourceManager;

	private:
		DefaultMeshRes() = delete;
		~DefaultMeshRes() = delete;

		static void Load();
	};
}