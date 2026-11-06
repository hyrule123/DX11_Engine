#include "Engine/Core/pch.h"
#include "ResourceManager.h"

#include <Engine/Resource/Resource.h>

#include <Engine/Resource/DefaultRes/DefaultRes.h>

#include <Engine/Core/Debug.h>

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
		// 1. 경로를 담을 버퍼 선언 (Windows 최대 경로 길이인 MAX_PATH 사용)
		wchar_t buffer[MAX_PATH];

		// 2. 현재 실행 파일(.exe)의 절대 경로를 버퍼에 채우기
		// 첫 번째 인자가 nullptr이면 현재 프로세스의 모듈을 의미합니다.
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);

		//path로 받아오고, weakly_canonical을 통해 절대 주소 만들기
		program_path_ = buffer;
		resource_dir_ = std::filesystem::weakly_canonical(program_path_.remove_filename() / ".." / ".." / ".." / "Res");

		return true;
	}
	void ResourceManager::LoadDefaultResources()
	{
		ASSERT_MAIN_THREAD();

		// ============ 기본 리소스 로드 ===============
		DefaultRes::LoadDefaultResources();
	}
	std::shared_ptr<Resource> ResourceManager::AddOrGetResource(const HashedStringView& res_key, std::shared_ptr<Resource> resource)
	{
		std::unique_lock lock(resources_mutex_);

		auto [iter, inserted] = resources_.cont.try_emplace(std::string(res_key.GetStringView()), std::move(resource));
		return iter->second;
	}
	std::shared_ptr<Resource> ResourceManager::Find(const HashedStringView& res_key)
	{
		std::shared_lock lock(resources_mutex_); // 공유 잠금(읽기)

		auto iter = resources_.find(res_key);
		if (iter != resources_.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	bool ResourceManager::AddResource(const HashedStringView& res_key, std::shared_ptr<Resource> resource)
	{
		std::unique_lock lock(resources_mutex_); // 쓰기 잠금

		// 읽기 시점에는 shared, 쓰기 시점에는 unique를 쓴다면?
		// -> 위험합니다. 검사와 삽입이 하나의 동작이 아니게 되기 때문입니다. 잠금을 바꾸는 사이에 다른 스레드가 끼어들 수 있습니다. 
		// 이런 오류를 TOCTOU(Time-of-check to time-of-use)라고 합니다.

		auto iter = resources_.find(res_key);
		if (iter != resources_.end())
		{
			ASSERT_F(false, "Resource already exists");
			return false;
		}

		resources_.insert(res_key, std::move(resource));
		return true;
	}
}