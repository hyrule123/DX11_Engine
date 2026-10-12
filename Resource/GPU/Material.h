#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constants.h>
#include <Engine/Core/Enum.h>

#include <Engine/Render/RenderTypes.h>

#include <Engine/Util/IDAllocator.h>
#include <Engine/Util/GPUBufferBindingTable.h>

#include <array>

struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace engine
{
    class PipelineState;
    class Texture2D;

    class Material :
        public Resource
    {
        ENTITY_INFO(Material, Resource)

    public:
		using PipelineStatesPerPass = std::array<std::shared_ptr<PipelineState>, (size_t)render_pass::ID::kCount>;

        Material();
        Material(const Material& other) = default;
        virtual ~Material() override;

        //고유 텍스처를 만들 때 싸용
        std::shared_ptr<Material> Clone() const { 
            return std::make_shared<Material>(*this);
        }

		bool IsReady(render_pass::ID pass) const {
			return (bool)pipeline_states_per_pass[(size_t)pass];
		}

        bool SetPipelineState(render_pass::ID pass, const HashedStringView& shader_set_name);
        void SetPipelineState(render_pass::ID pass, std::shared_ptr<PipelineState> shader_set);
        bool BindPipelineState(ID3D11DeviceContext* context, render_pass::ID pass) const;
		PipelineState* GetPipelineState(render_pass::ID pass) const {
            if (pass < render_pass::ID::kCount) { return pipeline_states_per_pass[(size_t)pass].get(); }
            return nullptr;
		}
		const PipelineStatesPerPass& GetPipelineStates() const { return pipeline_states_per_pass; }

        void Bind(ID3D11DeviceContext* context) const {
            per_material_binding_table_.Bind(context);
        }

        render_pass::Flags GetSupportedPasses() const { return supported_passes_; }

        void RemoveTexture(RegisterT slot) {
            per_material_binding_table_.RemoveShaderResource(slot);
        }

        void SetTexture(ShaderStageFlags stage_flag, RegisterT slot, std::shared_ptr<Texture2D> tex);
        bool SetTexture(ShaderStageFlags stage_flag, RegisterT slot, const HashedStringView& texture_name);

		bool IsInstancingSupported(render_pass::ID pass) const;
        size_t GetInstanceDataStride(render_pass::ID pass) const;

		uint32 GetMaterialID() const { return material_ID_.Get(); }

    private:
		MaterialID material_ID_;    // RenderKey에 패킹되는 Material 고유 ID. ScopedID로 관리됨
        GPUBufferBindingTable per_material_binding_table_;

        PipelineStatesPerPass pipeline_states_per_pass = {};

        render_pass::Flags supported_passes_ = {};
    };
}