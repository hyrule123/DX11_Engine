#include "Engine/pch.h"
#include "ResourceManager.h"

#include <Engine/Resource/Mesh/DefaultMesh.h>

namespace engine
{
	ResourceManager::ResourceManager()
	{

	}

	ResourceManager::~ResourceManager()
	{

	}

	bool ResourceManager::Init()
	{
		DefaultMesh::LoadDefaultMesh();

		return true;
	}
	s_ptr<Resource> ResourceManager::Find(const stdfs::path& path)
	{
		auto iter = resources_.find(path);

		if (iter != resources_.end())
		{
			return iter->second;
		}

		return nullptr;
	}
	bool ResourceManager::AddResource(const stdfs::path& path, s_ptr<Resource> resource)
	{
		auto iter = resources_.find(path);
		if (iter != resources_.end())
		{
			return false;
		}

		resources_[path] = resource;
		return true;
	}
}