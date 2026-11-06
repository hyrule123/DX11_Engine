#pragma once

#include <Engine/Core/Singleton.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Entity.h>

#include <unordered_set>

// 쓰기: unique_lock, 읽기: shared_lock
#include <shared_mutex>

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
		std::shared_ptr<T> Load(const HashedStringView& res_key)
		{
			std::shared_ptr<T> resource = Find<T>(res_key);
			if (resource) { return resource; }

			// 잠금 밖에서 로드: 로드 중 다른 리소스를 재귀적으로 로드할 수 있음
			resource = LoadUncached<T>(res_key);
			if (resource == nullptr) { return nullptr; }

			return std::dynamic_pointer_cast<T>(AddOrGetResource(res_key, resource));
		}

		template <typename T>
		std::shared_ptr<T> LoadUncached(const HashedStringView& res_key)
		{
			static_assert(std::is_base_of_v<Resource, T>, "T must be derived from Resource");
			std::shared_ptr<T> resource = EntityManager::CreateEntity<T>();
			if (false == resource->Load(resource_dir_ / res_key.GetStringView()))
			{
				return nullptr;
			}
			return resource;
		}

		bool AddResource(const HashedStringView& res_key, std::shared_ptr<Resource> resource);

		void SetDefaultResource(std::shared_ptr<Resource> resource)
		{
			ASSERT_MAIN_THREAD();
			default_resources_.insert(resource);
		}

		const std::filesystem::path& GetProgramPath() const { return program_path_; }
		const std::filesystem::path& GetResourceDir() const { return resource_dir_; }

	private:
		bool Init();
		void LoadDefaultResources();

		// 이미 같은 키가 있으면 기존 리소스를 반환 (동시 로드 경쟁 시 먼저 삽입한 쪽이 유지됨)
		std::shared_ptr<Resource> AddOrGetResource(const HashedStringView& res_key, std::shared_ptr<Resource> resource);

	private:
		StringHashTable<std::shared_ptr<Resource>> resources_;
		std::unordered_set<std::shared_ptr<Resource>> default_resources_;

		std::filesystem::path program_path_ = {};
		std::filesystem::path resource_dir_ = {};

		std::shared_mutex resources_mutex_ = {};  // resources_ 보호. 잠근 채로 리소스를 로드하지 말 것
	};
}


