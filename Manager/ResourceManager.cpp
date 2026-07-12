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
		resource_dir_ = stdfs::weakly_canonical(program_path_.remove_filename() / ".." / ".." / ".." / "Res");

		return true;
	}
	void ResourceManager::LoadDefaultResources()
	{
		// ============ 기본 리소스 로드 ===============
		DefaultRes::LoadDefaultResources();
	}
	s_ptr<Resource> ResourceManager::Find(const HashedStringView& res_key)
	{
		s_ptr<Resource> ret = nullptr;
		auto iter = resources_.find(res_key);
		if (iter != resources_.end()) { ret = iter->second; }
		return ret;
	}
	bool ResourceManager::AddResource(const HashedStringView& res_key, s_ptr<Resource> resource)
	{
		auto iter = resources_.find(res_key);
		if (iter != resources_.end()) 
		{ 
			DEBUG_MESSAGE("Resource already exists");
			return false; 
		}

		resources_.insert(res_key, std::move(resource));
		return true;
	}
}