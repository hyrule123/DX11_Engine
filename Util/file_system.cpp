#include "Engine/Core/pch.h"
#include "file_system.h"

#include <fstream>

namespace engine::file_system
{
	std::vector<uint8> ReadAllBytes(const stdfs::path& file_path)
	{
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);

        std::vector<uint8_t> buffer;
        if (false == file.is_open()) {
            ERROR_MESSAGE("파일 열기 실패!!");
            return buffer;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer.resize(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
	}
}