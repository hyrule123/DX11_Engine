#include "Engine/pch.h"
#include "Shader.h"

namespace engine
{
	Shader::Shader(const std::string_view class_name)
		: Resource(class_name)
	{}
	Shader::~Shader()
	{}
}