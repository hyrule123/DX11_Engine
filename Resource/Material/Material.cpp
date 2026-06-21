#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Resource/GraphicsPipeline/GraphicsPipeline.h>
#include <Engine/Resource/Texture2D.h>

namespace engine
{
	Material::Material()
		: Resource(STRINGIFY(Material))
	{
	}

	Material::~Material()
	{
	}

	void Material::Bind(ID3D11DeviceContext* context)
	{
		std::array<ID3D11ShaderResourceView*, kTextureMaxCount> srvs = {};
		for (size_t i = 0; i < textures_.size(); ++i)
		{
			if (textures_[i])
			{
				srvs[i] = textures_[i]->GetSRV();
			}
		}
		Texture2D::BindTextures(context, srvs, ShaderStage::kPS);

		if (pipeline_)
		{
			pipeline_->Bind(context);
		}
	}
}

