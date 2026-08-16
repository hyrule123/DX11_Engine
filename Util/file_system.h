#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <filesystem>
#include <fstream>
#include <vector>

namespace engine
{
	namespace file_system
	{
		std::vector<uint8> ReadAllBytes(const stdfs::path& file_path);
		std::vector<uint8> ReadAllBytes(std::ifstream& ifs);

		template <typename T>
		std::vector<T> ReadAll(std::ifstream& fs)
		{
			fs.seekg(0, std::ios::end);
			const std::streamoff bytes = fs.tellg();
			fs.seekg(0, std::ios::beg);

			if (bytes <= 0 || bytes % sizeof(T) != 0)
			{
				return {};
			}

			std::vector<T> out((size_t)bytes / sizeof(T));
			fs.read((char*)out.data(), bytes);
			return out;
		}

		bool DumpFile(const stdfs::path& file_path, const std::vector<uint8>& data);
	}
}