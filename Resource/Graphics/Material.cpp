#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/GraphicsPipeline.h>
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

namespace engine
{
	Material::Material()
		: Resource(STRINGIFY(Material))
	{
	}

	Material::~Material()
	{
	}

	bool Material::SetGraphicsPipeline(const stdfs::path& pipeline_name)
	{
		pipeline_ = ResourceManager::GetInst().Find<GraphicsPipeline>(pipeline_name);
		return (bool)pipeline_;
	}

	void Material::Bind(ID3D11DeviceContext* context)
	{
		std::array<ID3D11ShaderResourceView*, kMaxTextureCount> srvs = {};
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
	bool Material::SetTexture(const stdfs::path& texture_filepath, uint32 slot)
	{
		s_ptr<Texture2D> tex = 
			ResourceManager::GetInst().LoadFromFile<Texture2D>(texture_filepath);

		if (tex)
		{
			SetTexture(tex, slot);
			return true;
		}

		return false;
	}
	void Material::SetTexture(s_ptr<Texture2D> tex, uint32 slot)
	{
		if ((size_t)slot < textures_.size()) {
			textures_[slot] = tex;
			if (tex)
			{
				srv_cache_[slot] = tex->GetSRV();
			}
			else
			{
				srv_cache_[slot] = nullptr;
			}
		}
	}
}

