#include "Engine/pch.h"
#include "FileSystem.h"

#include <fstream>

namespace engine
{
	std::vector<uint8> FileSystem::ReadAllBytes(const stdfs::path& file_path)
	{
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);

        std::vector<uint8_t> buffer;
        if (false == file.is_open()) {
            return buffer;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer.resize(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
	}
}