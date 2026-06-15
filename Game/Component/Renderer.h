#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>

namespace engine
{
    class Material;
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

        void SetMaterial(s_ptr<Material> material) { material_ = std::move(material); }
        void SetMesh(s_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }

    private:
        s_ptr<Material> material_ = {};
        s_ptr<Mesh> mesh_ = {};
    };
}



