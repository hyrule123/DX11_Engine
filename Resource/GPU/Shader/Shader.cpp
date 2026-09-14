#include "Engine/Core/pch.h"
#include "Shader.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Util/file_system.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	Shader::Shader(const HashedStringView& concrete_class_name)
		: Resource(concrete_class_name)
	{}
	Shader::~Shader()
	{}
	bool Shader::LoadFromFile(const stdfs::path & path)
	{
		std::vector<uint8> bytecode = file_system::ReadAllBytes(path);

		if (bytecode.empty())
		{
			ERR_MSG("Failed to Load Shader From file: {}", path.string());
			return false;
		}

		return Create(bytecode);
	}
}