#include "Engine/Core/pch.h"
#include "Material.h"

#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/PipelineState.h>
#include <Engine/Resource/GPU/Buffer/Texture2D.h>

#include <Engine/Core/Debug.h>

#include <Engine/Util/IDAllocator.h>

#include <Engine/HLSL/Core/Config.hlsli>
#include <Engine/HLSL/Core/Register.hlsli>

#include <utility>

namespace engine
{
	Material::Material()
		: Resource(Material::kClassConcreteName)
		, material_ID_()	// 기본 생성자에서 발급됨(RAII)
		, per_material_binding_table_(REG_B_PER_MATERIAL_START, REG_B_PER_MATERIAL_COUNT, REG_T_PER_MATERIAL_START, REG_T_PER_MATERIAL_COUNT)
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

	void Material::SetPipelineState(RenderPassOrder pass, std::shared_ptr<PipelineState> shader_set)
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
		
		ASSERT_F(false, "PipelineState is not set for the given RenderPassOrder.");
		PipelineState::Clear(context);
		return false;
	}

	void Material::SetTexture(ShaderStageFlags stage_flag, RegisterT slot, std::shared_ptr<Texture2D> tex)
	{
		if (tex)
		{
			per_material_binding_table_.AddShaderResource(stage_flag, slot, std::move(tex));
		}
		else
		{
			DEBUG_LOG("Material::SetTexture: nullptr texture for slot {}, removing binding.", slot.Get());
			per_material_binding_table_.RemoveShaderResource(slot);
		}
	}

	bool Material::SetTexture(ShaderStageFlags stage_flag, RegisterT slot, const HashedStringView& texture_name)
	{
		std::shared_ptr<Texture2D> tex =
			ResourceManager::GetInst().LoadFromFile<Texture2D>(texture_name);

		if (tex == nullptr) { return false; }

		SetTexture(stage_flag, slot, std::move(tex));

		return true;
	}
	bool Material::IsInstancingSupported(RenderPassOrder pass) const
	{
		if (pipeline_states_per_pass[(size_t)pass])
		{
			return pipeline_states_per_pass[(size_t)pass]->IsInstancingSupported();
		}
		return false;
	}
	size_t Material::GetPerObjectDataStride(RenderPassOrder pass) const
	{
		if (pipeline_states_per_pass[(size_t)pass])
		{
			return pipeline_states_per_pass[(size_t)pass]->GetPerInstanceDataStride();
		}
		return 0;
	}
}

