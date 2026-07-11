#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Util/StringHashTable.h>

#include <unordered_set>

namespace engine
{
	class Resource;
	class ResourceManager
	{
		DECLARE_SINGLETON(ResourceManager)

		friend class GameEngine;

	public:
		s_ptr<Resource> Find(const HashedStringView& res_key);

		template <typename T>
		s_ptr<T> Find(const HashedStringView& res_key)
		{
			s_ptr<Resource> result = Find(res_key);
			if (result)
			{
				return std::dynamic_pointer_cast<T>(result);
			}
			return nullptr;
		}

		template <typename T>
		s_ptr<T> LoadFromFile(const HashedStringView& res_key)
		{
			s_ptr<T> resource = Find<T>(res_key);
			if (resource) { return resource; }
			resource = LoadFromFileWithoutAdd<T>(res_key);
			if (resource) 
			{ 
				resources_.insert(std::make_pair(std::string(res_key.GetStringView()), resource));
			}
			return resource;
		}

		template <typename T>
		s_ptr<T> LoadFromFileWithoutAdd(const HashedStringView& res_key)
		{
			s_ptr<T> resource = std::make_shared<T>();
			if (false == resource->LoadFromFile(resource_dir_ / res_key.GetStringView()))
			{
				return nullptr;
			}
			return resource;
		}

		bool AddResource(const HashedStringView& res_key, s_ptr<Resource> resource);

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
		StringHashTable<s_ptr<Resource>> resources_;
		std::unordered_set<s_ptr<Resource>> default_resources_;

		stdfs::path program_path_ = {};
		stdfs::path resource_dir_ = {};
	};
}


