#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>

namespace engine
{
    class Transform;
    class Material;
    class Mesh;
    class ConstantBuffer;

    class Renderer 
        : public Component
    {
        CLASS_INFO(Renderer, Component)
        COMPONENT_CATEGORY(ComponentCategory::kRenderer)
    public:
        Renderer();
        virtual ~Renderer() override;

        virtual void Init() override;
        virtual void Awake() override;
        virtual void LateUpdate() override;

        s_ptr<Transform> GetTransform() const { return my_transform_; }
        void SetMaterial(s_ptr<Material> material) { material_ = std::move(material); }
        void SetMesh(s_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }

        bool IsRenderReady() const { return (material_ && mesh_); }

        s_ptr<Material> GetMaterial() const { return material_; }
        s_ptr<Mesh> GetMesh() const { return mesh_; }

    private:
        s_ptr<Transform> my_transform_ = {};

        s_ptr<Material> material_ = {};
        s_ptr<Mesh> mesh_ = {};

        s_ptr<ConstantBuffer> per_obj_cbuffer_ = {};
    };
}



