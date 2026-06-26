#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>

#include <array>

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
        Material();
        virtual ~Material() override;

        bool SetGraphicsPipeline(const stdfs::path& pipeline_name);
        void SetGraphicsPipeline(s_ptr<GraphicsPipeline> pipeline) {
            pipeline_ = std::move(pipeline);
        }

        void Bind(ID3D11DeviceContext* context);

        bool SetTexture(const stdfs::path& texture_filepath, uint32 slot);
        void SetTexture(s_ptr<Texture2D> tex, uint32 slot);

    private:
        std::array<s_ptr<Texture2D>, kMaxTextureCount> textures_ = {};
        std::array<ID3D11ShaderResourceView*, kMaxTextureCount> srv_cache_ = {};

        s_ptr<GraphicsPipeline> pipeline_ = {};
    };
}


