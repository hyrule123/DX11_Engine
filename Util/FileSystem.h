#pragma once

#include <Engine/Common.h>

namespace engine
{
	class FileSystem
	{
		static std::vector<uint8> ReadAllBytes(const stdfs::path& file_path);

	private:
		FileSystem() = delete;
		~FileSystem() = delete;
	};

}