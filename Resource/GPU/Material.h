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
		using PipelineStatesPerPass = std::array<std::shared_ptr<PipelineState>, (size_t)RenderPassOrder::kCount>;

        Material();
        Material(const Material& other) = default;
        virtual ~Material() override;

        //고유 텍스처를 만들 때 싸용
        std::shared_ptr<Material> Clone() const { 
            return std::make_shared<Material>(*this);
        }

		bool IsReady(RenderPassOrder pass) const {
			return (bool)pipeline_states_per_pass[(size_t)pass];
		}

        bool SetPipelineState(RenderPassOrder pass, const HashedStringView& shader_set_name);
        void SetPipelineState(RenderPassOrder pass, std::shared_ptr<PipelineState> shader_set);
        bool BindPipelineState(ID3D11DeviceContext* context, RenderPassOrder pass);
		PipelineState* GetPipelineState(RenderPassOrder pass) const {
            if (pass < RenderPassOrder::kCount) { return pipeline_states_per_pass[(size_t)pass].get(); }
            return nullptr;
		}
		const PipelineStatesPerPass& GetPipelineStates() const { return pipeline_states_per_pass; }

        void BindPerMaterialBuffers(ID3D11DeviceContext* context) {
            per_material_binding_table_.Bind(context);
        }

        void RemoveTexture(RegisterT slot) {
            per_material_binding_table_.RemoveShaderResource(slot);
        }

        void SetTexture(ShaderStageFlags stage_flag, RegisterT slot, std::shared_ptr<Texture2D> tex);
        bool SetTexture(ShaderStageFlags stage_flag, RegisterT slot, const HashedStringView& texture_name);

		bool IsInstancingSupported(RenderPassOrder pass) const;
        size_t GetPerObjectDataStride(RenderPassOrder pass) const;

		uint32 GetMaterialID() const { return material_ID_.Get(); }

    private:
		MaterialID material_ID_;    // RenderKey에 패킹되는 Material 고유 ID. ScopedID로 관리됨
        GPUBufferBindingTable per_material_binding_table_;

        // TODO: 이후 Per Material 버퍼 추가 필요 시 추가해야 함

        PipelineStatesPerPass pipeline_states_per_pass = {};
    };
}


