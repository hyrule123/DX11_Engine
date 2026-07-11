#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/Graphics/GraphicsShaderSet.h>
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	Material::Material()
		: Resource(STRINGIFY(Material))
	{
	}

	Material::~Material()
	{
	}

	bool Material::SetShaderSet(const HashedStringView& shader_set_name, RenderPassOrder pass)
	{
		SetShaderSet(ResourceManager::GetInst().Find<GraphicsShaderSet>(shader_set_name), pass);
		return (bool)shader_sets_per_pass_[(size_t)pass];
	}

	void Material::SetShaderSet(s_ptr<GraphicsShaderSet> shader_set, RenderPassOrder pass)
	{
		if (shader_set)
		{
			ASSERT(shader_set->IsReady());
			shader_sets_per_pass_[(size_t)pass] = std::move(shader_set);
		}
		else
		{
			shader_sets_per_pass_[(size_t)pass] = nullptr;
		}
	}

	bool Material::BindShaderSet(ID3D11DeviceContext* context, RenderPassOrder pass)
	{
		if (shader_sets_per_pass_[(size_t)pass]) 
		{ 
			shader_sets_per_pass_[(size_t)pass]->Bind(context); 
			return true; 
		}
		
		DEBUG_BREAK
		GraphicsShaderSet::Clear(context);
		return false;
	}

	void Material::BindTextures(ID3D11DeviceContext* context, ShaderStage::Flags stage_flag)
	{
		Texture2D::BindSRVs(context, srv_cache_, stage_flag);
	}

	bool Material::SetTexture(const HashedStringView& texture_name, uint32 slot)
	{
		s_ptr<Texture2D> tex = 
			ResourceManager::GetInst().LoadFromFile<Texture2D>(texture_name);

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
	bool Material::IsInstancingSupported(RenderPassOrder pass) const
	{
		if (shader_sets_per_pass_[(size_t)pass])
		{
			return shader_sets_per_pass_[(size_t)pass]->IsInstancingSupported();
		}
		return false;
	}
	size_t Material::GetInstanceDataStride(RenderPassOrder pass) const
	{
		if (shader_sets_per_pass_[(size_t)pass])
		{
			return shader_sets_per_pass_[(size_t)pass]->GetPerInstanceDataStride();
		}
		return 0;
	}
}

