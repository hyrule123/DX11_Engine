#pragma once
#include <Engine/Resource/Resource.h>

namespace engine
{
    class GraphicsPipeline;

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

    private:
        s_ptr<GraphicsPipeline> pipeline_ = {};
    };
}


