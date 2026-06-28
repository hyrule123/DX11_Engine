#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>

#include <array>

struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace engine
{
    class GraphicsPipeline;
    class Texture2D;

    class Material :
        public Resource
    {
        CLASS_INFO(Material, Resource)

    public:
        using Textures = std::array<s_ptr<Texture2D>, kMaxTextureCount>;

        Material();
        Material(const Material& other) = default;
        virtual ~Material() override;

        s_ptr<Material> Clone() const { 
            return std::make_shared<Material>(*this); 
        }

        bool SetGraphicsPipeline(const stdfs::path& pipeline_name);
        void SetGraphicsPipeline(s_ptr<GraphicsPipeline> pipeline) {
            pipeline_ = std::move(pipeline);
        }

        void BindTextures(ID3D11DeviceContext* context, ShaderStageFlag stage_flag);
        void BindGraphicsPipeline(ID3D11DeviceContext* context);
        void BindAll(ID3D11DeviceContext* context, ShaderStageFlag stage_flag);

        bool SetTexture(const stdfs::path& texture_filepath, uint32 slot);
        void SetTexture(s_ptr<Texture2D> tex, uint32 slot);
        void SetTextures(const Textures& textures) {
            for (size_t i = 0; i < textures.size(); ++i)
            {
                SetTexture(textures[i], (uint32)i);
            }
        }

    private:
        Textures textures_ = {};
        std::array<ID3D11ShaderResourceView*, kMaxTextureCount> srv_cache_ = {};

        s_ptr<GraphicsPipeline> pipeline_ = {};
    };
}


