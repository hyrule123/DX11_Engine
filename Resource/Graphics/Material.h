#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>

#include <array>

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

        void SetGraphicsPipeline(s_ptr<GraphicsPipeline> pipeline) {
            pipeline_ = std::move(pipeline);
        }

        void Bind(ID3D11DeviceContext* context);

        void SetTexture(s_ptr<Texture2D> tex, size_t slot) {
            if (slot < textures_.size()) { textures_[slot] = tex; }
        }

    private:
        std::array<s_ptr<Texture2D>, kMaxTextureCount> textures_ = {};

        s_ptr<GraphicsPipeline> pipeline_ = {};
    };
}


