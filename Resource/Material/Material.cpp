#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Resource/GraphicsPipeline/GraphicsPipeline.h>

namespace engine
{
	Material::Material()
		: Resource(STRINGIFY(Material))
	{
	}

	Material::~Material()
	{
	}
	void Material::Bind()
	{
		if (pipeline_)
		{
			pipeline_->Bind();
		}
	}
}

