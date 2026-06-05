#include "Engine/pch.h"
#include "Resource.h"

namespace engine
{
	Resource::Resource(const std::string_view class_name)
		: Entity(class_name)
	{}
	Resource::~Resource()
	{}
	bool Resource::LoadFromFile(const stdfs::path & path)
	{
		res_path_ = path;
		return true;
	}
}