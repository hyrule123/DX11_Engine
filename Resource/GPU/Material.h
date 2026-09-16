#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Enum.h>

#include <Engine/Render/RenderTypes.h>

#include <Engine/Util/IDAllocator.h>

#include <Engine/HLSL/Core/Config.hlsli>

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
        using Textures = std::array<s_ptr<Texture2D>, MAX_TEXTURE_COUNT>;
		using PipelineStatesPerPass = std::array<s_ptr<PipelineState>, (size_t)RenderPassOrder::kCount>;

        Material();
        Material(const Material& other) = default;
        virtual ~Material() override;

        //고유 텍스처를 만들 때 싸용
        s_ptr<Material> Clone() const { 
            return std::make_shared<Material>(*this);
        }

		bool IsReady(RenderPassOrder pass) const {
			return (bool)pipeline_states_per_pass[(size_t)pass];
		}

        bool SetPipelineState(RenderPassOrder pass, const HashedStringView& shader_set_name);
        void SetPipelineState(RenderPassOrder pass, s_ptr<PipelineState> shader_set);
        bool BindPipelineState(ID3D11DeviceContext* context, RenderPassOrder pass);
		PipelineState* GetPipelineState(RenderPassOrder pass) const {
            if (pass < RenderPassOrder::kCount) { return pipeline_states_per_pass[(size_t)pass].get(); }
            return nullptr;
		}
		const PipelineStatesPerPass& GetPipelineStates() const { return pipeline_states_per_pass; }

        // Material Start slot으로부터 8장 연속으로 바인딩함. 슬롯 주의
        void BindTextures(ID3D11DeviceContext* context, ShaderStageFlags stage_flag = ShaderStageFlags(ShaderStage::Pixel));

        bool SetTexture(RegisterT slot, const HashedStringView& texture_name);
        void SetTexture(RegisterT slot, s_ptr<Texture2D> tex);
        void SetTextures(const Textures& textures) { textures_ = textures; }

		bool IsInstancingSupported(RenderPassOrder pass) const;
        size_t GetPerObjectDataStride(RenderPassOrder pass) const;

		MaterialID GetMaterialID() const { return material_ID_; }

    private:
		MaterialID material_ID_;    // RenderKey에 패킹되는 Material 고유 ID. ScopedID로 관리됨

        Textures textures_ = {};

        // TODO: 이후 Per Material 버퍼 추가 필요 시 추가해야 함

        PipelineStatesPerPass pipeline_states_per_pass = {};
    };
}


