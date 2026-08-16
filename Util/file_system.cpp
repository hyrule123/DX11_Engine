#include "Engine/Core/pch.h"
#include "file_system.h"

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Debug.h>

#include <fstream>

namespace engine::file_system
{
	std::vector<uint8> ReadAllBytes(const stdfs::path& file_path)
	{
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        
        if (false == file.is_open()) {
            ERROR_MESSAGE("파일 열기 실패!!");
            return {};
        }

		return ReadAllBytes(file);
	}
    std::vector<uint8> ReadAllBytes(std::ifstream& ifs)
    {
        std::vector<uint8> buffer;

        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        buffer.resize(size);
        ifs.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
    }
    bool DumpFile(const stdfs::path& file_path, const std::vector<uint8>& data)
    {
		std::ofstream of(file_path, std::ios::binary);
        
        if (false == of.is_open())
        {
			ERROR_MESSAGE("파일 열기 실패!!");
			return false;
        }

        of.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }
}