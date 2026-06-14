#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/Type.h>

namespace engine
{
    class Mesh;

    class Renderer 
        : public Component
    {
        CLASS_INFO(Renderer, Component)
        COMPONENT_CATEGORY(ComponentCategory::kRenderer)
    public:
        Renderer();
        virtual ~Renderer() override;

        virtual void Init() override;
        virtual void Render() override;

        void SetMesh(s_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }

    private:
        s_ptr<class InputLayout> input_layout_ = {};
        s_ptr<class VertexShader> vs_ = {};
        s_ptr<class PixelShader> ps_ = {};

        s_ptr<Mesh> mesh_ = {};
    };
}



