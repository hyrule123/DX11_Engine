#pragma once

#include <Engine/Common.h>
#include <Engine/Singleton.h>

#include <unordered_set>

namespace engine
{
	class Resource;
	class ResourceManager
	{
		DECLARE_SINGLETON(ResourceManager)

		friend class GameEngine;

	public:
		s_ptr<Resource> Find(const stdfs::path& path);

		template <typename T>
		s_ptr<T> Find(const stdfs::path& path)
		{
			s_ptr<Resource> result = Find(path);
			if (result)
			{
				return std::dynamic_pointer_cast<T>(result);
			}
			return nullptr;
		}

		template <typename T>
		s_ptr<T> LoadFromFile(const stdfs::path& path)
		{
			s_ptr<T> resource = Find<T>(path);
			if (resource) { return resource; }

			resource = std::make_shared<T>();
			if (false == resource->LoadFromFile(path))
			{ 
				return nullptr; 
			}
				
			resources_[path] = resource;
			return resource;
		}

		bool AddResource(const stdfs::path& path, s_ptr<Resource> resource);

		void SetDefaultResource(s_ptr<Resource> resource)
		{
			default_resources_.insert(resource);
		}

	private:
		bool Init();


	private:
		std::unordered_map <stdfs::path, s_ptr<Resource>> resources_;

		std::unordered_set<s_ptr<Resource>> default_resources_;
	};
}


