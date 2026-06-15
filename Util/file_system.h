#pragma once

#include <Engine/Core/CoreMinimal.h>

namespace engine
{
	namespace file_system
	{
		std::vector<uint8> ReadAllBytes(const stdfs::path& file_path);
	}
}