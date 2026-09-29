#pragma once

#include <Engine/Core/Singleton.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Entity.h>

#include <unordered_set>

namespace engine
{
	class Resource;
	class ResourceManager
	{
		DECLARE_SINGLETON(ResourceManager)

		friend class GameEngine;

	public:
		std::shared_ptr<Resource> Find(const HashedStringView& res_key);

		template <typename T>
		std::shared_ptr<T> Find(const HashedStringView& res_key)
		{
			std::shared_ptr<Resource> result = Find(res_key);
			if (result)
			{
				return std::dynamic_pointer_cast<T>(result);
			}
			return nullptr;
		}

		template <typename T>
		std::shared_ptr<T> LoadFromFile(const HashedStringView& res_key)
		{
			std::shared_ptr<T> resource = Find<T>(res_key);
			if (resource) { return resource; }
			resource = LoadFromFileWithoutAdd<T>(res_key);
			if (resource) 
			{ 
				resources_.insert(res_key.GetStringView(), resource);
			}
			return resource;
		}

		template <typename T>
		std::shared_ptr<T> LoadFromFileWithoutAdd(const HashedStringView& res_key)
		{
			static_assert(std::is_base_of_v<Resource, T>, "T must be derived from Resource");
			std::shared_ptr<T> resource = EntityManager::CreateEntity<T>();
			if (false == resource->LoadFromFile(resource_dir_ / res_key.GetStringView()))
			{
				return nullptr;
			}
			return resource;
		}

		bool AddResource(const HashedStringView& res_key, std::shared_ptr<Resource> resource);

		void SetDefaultResource(std::shared_ptr<Resource> resource)
		{
			default_resources_.insert(resource);
		}

		const std::filesystem::path& GetProgramPath() const { return program_path_; }
		const std::filesystem::path& GetResourceDir() const { return resource_dir_; }

	private:
		bool Init();
		void LoadDefaultResources();

	private:
		StringHashTable<std::shared_ptr<Resource>> resources_;
		std::unordered_set<std::shared_ptr<Resource>> default_resources_;

		std::filesystem::path program_path_ = {};
		std::filesystem::path resource_dir_ = {};
	};
}


