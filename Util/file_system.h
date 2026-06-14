#pragma once

#include <Engine/Core/Type.h>

#include <filesystem>
#include <vector>

namespace engine
{
	namespace file_system
	{
		std::vector<uint8> ReadAllBytes(const stdfs::path& file_path);
	}
}