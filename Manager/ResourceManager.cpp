#include "Engine/pch.h"
#include "ResourceManager.h"

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


		return true;
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