#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/PipelineState.h>
#include <Engine/Resource/GPU/Buffer/Texture2D.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/Core/Register.hlsli>

#include <utility>

namespace engine
{
	Material::Material()
		: Resource(Material::kClassConcreteName)
	{
	}

	Material::~Material()
	{
	}

	bool Material::SetPipelineState(RenderPassOrder pass, const HashedStringView& shader_set_name)
	{
		SetPipelineState(pass, ResourceManager::GetInst().Find<PipelineState>(shader_set_name));
		return (bool)pipeline_states_per_pass[(size_t)pass];
	}

	void Material::SetPipelineState(RenderPassOrder pass, s_ptr<PipelineState> shader_set)
	{
		if (shader_set)
		{
			ASSERT(shader_set->IsReady());
			pipeline_states_per_pass[(size_t)pass] = std::move(shader_set);
		}
		else
		{
			pipeline_states_per_pass[(size_t)pass] = nullptr;
		}
	}

	bool Material::BindPipelineState(ID3D11DeviceContext* context, RenderPassOrder pass)
	{
		if (pipeline_states_per_pass[(size_t)pass]) 
		{ 
			pipeline_states_per_pass[(size_t)pass]->Bind(context); 
			return true; 
		}
		
		DEBUG_BREAK
		PipelineState::Clear(context);
		return false;
	}

	void Material::BindTextures(ID3D11DeviceContext* context, ShaderStage::Flags stage_flag)
	{
		std::array<ID3D11ShaderResourceView*, std::tuple_size_v<Textures>> srv = {};
		for (size_t i = 0; i < textures_.size(); ++i)
		{
			if (textures_[i])
			{
				srv[i] = textures_[i]->GetRawSRV();
			}
		}

		constexpr UINT max_tex_count = (UINT)std::tuple_size_v<Textures>;
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Vertex))
		{
			context->VSSetShaderResources(REG_T_PER_MATERIAL_START, max_tex_count, srv.data());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Geometry))
		{
			context->GSSetShaderResources(REG_T_PER_MATERIAL_START, max_tex_count, srv.data());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Pixel))
		{
			context->PSSetShaderResources(REG_T_PER_MATERIAL_START, max_tex_count, srv.data());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Compute))
		{
			context->CSSetShaderResources(REG_T_PER_MATERIAL_START, max_tex_count, srv.data());
		}
	}

	void Material::SetTexture(RegisterT slot, s_ptr<Texture2D> tex)
	{
		int32 slot_idx = (int32)slot.Get() - (int32)REG_T_PER_MATERIAL_START;
		if (slot_idx < 0 || MAX_TEXTURE_COUNT <= slot_idx)
		{
			ASSERT_RELEASE("Material::SetTexture() - Invalid slot index");
			return;
		}
		textures_[slot_idx] = std::move(tex);
	}

	bool Material::SetTexture(RegisterT slot, const HashedStringView& texture_name)
	{
		s_ptr<Texture2D> tex = 
			ResourceManager::GetInst().LoadFromFile<Texture2D>(texture_name);

		if (tex)
		{
			SetTexture(slot, tex);
			return true;
		}

		return false;
	}
	bool Material::IsInstancingSupported(RenderPassOrder pass) const
	{
		if (pipeline_states_per_pass[(size_t)pass])
		{
			return pipeline_states_per_pass[(size_t)pass]->IsInstancingSupported();
		}
		return false;
	}
	size_t Material::GetInstanceDataStride(RenderPassOrder pass) const
	{
		if (pipeline_states_per_pass[(size_t)pass])
		{
			return pipeline_states_per_pass[(size_t)pass]->GetPerInstanceDataStride();
		}
		return 0;
	}
}

