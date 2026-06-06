#pragma once



namespace engine
{
	class Resource;

	class DefaultMesh
	{
		friend class ResourceManager;

	private:
		DefaultMesh() = delete;
		~DefaultMesh() = delete;

		static void LoadDefaultMesh();
	};
}