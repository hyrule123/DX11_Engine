#include "Engine/Core/pch.h"
#include "Shader.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Util/file_system.h>

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
			return false;
		}

		return Create(bytecode);
	}
}