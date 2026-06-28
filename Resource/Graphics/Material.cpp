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

	void Material::BindTextures(ID3D11DeviceContext* context, ShaderStageFlag stage_flag)
	{
		Texture2D::BindSRVs(context, srv_cache_, stage_flag);
	}

	void Material::BindGraphicsPipeline(ID3D11DeviceContext* context)
	{
		if (pipeline_)
		{
			pipeline_->Bind(context);
		}
	}
	void Material::BindAll(ID3D11DeviceContext* context, ShaderStageFlag stage_flag)
	{
		BindTextures(context, stage_flag);
		BindGraphicsPipeline(context);
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
				srv_cache_[slot] = tex->GetRawSRV();
			}
			else
			{
				srv_cache_[slot] = nullptr;
			}
		}
	}
}

