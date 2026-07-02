#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>

#include <unordered_map>
#include <unordered_set>

namespace engine
{
	class Resource;
	class ResourceManager
	{
		DECLARE_SINGLETON(ResourceManager)

		friend class GameEngine;

	public:
		s_ptr<Resource> Find(const stdfs::path& res_relative_path);

		template <typename T>
		s_ptr<T> Find(const stdfs::path& res_relative_path)
		{
			s_ptr<Resource> result = Find(res_relative_path);
			if (result)
			{
				return std::dynamic_pointer_cast<T>(result);
			}
			return nullptr;
		}

		template <typename T>
		s_ptr<T> LoadFromFile(const stdfs::path& res_relative_path)
		{
			s_ptr<T> resource = Find<T>(res_relative_path);
			if (resource) { return resource; }
			resource = LoadFromFileWithoutAdd<T>(res_relative_path);
			if (resource) { resources_[res_relative_path] = resource; }
			return resource;
		}

		template <typename T>
		s_ptr<T> LoadFromFileWithoutAdd(const stdfs::path& res_relative_path)
		{
			s_ptr<T> resource = std::make_shared<T>();
			if (false == resource->LoadFromFile(resource_dir_ / res_relative_path))
			{
				return nullptr;
			}
			resource->SetResKey(res_relative_path);
			return resource;
		}

		bool AddResource(const stdfs::path& res_relative_path, s_ptr<Resource> resource);

		void SetDefaultResource(s_ptr<Resource> resource)
		{
			default_resources_.insert(resource);
		}

		const stdfs::path& GetProgramPath() const { return program_path_; }
		const stdfs::path& GetResourceDir() const { return resource_dir_; }

	private:
		bool Init();
		void LoadDefaultResources();

	private:
		std::unordered_map <stdfs::path, s_ptr<Resource>> resources_;
		std::unordered_set<s_ptr<Resource>> default_resources_;

		stdfs::path program_path_ = {};
		stdfs::path resource_dir_ = {};
	};
}


