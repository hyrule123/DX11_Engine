#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

#include <Engine/HLSL/CppShared/Struct.hlsli>

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
        Renderer(const std::string_view concrete_class_name);
        virtual ~Renderer() override;

        virtual void Init() override;
        virtual void Awake() override;
        virtual void LateUpdate() = 0;

        s_ptr<Transform> GetTransform() const { return my_transform_; }
        void SetMaterial(s_ptr<Material> material) { material_ = std::move(material); }
        bool SetMaterial(const HashedStringView& mtrl_name);
        void SetMesh(s_ptr<Mesh> mesh) { mesh_ = std::move(mesh); }
        bool SetMesh(const HashedStringView& mesh_name);

        bool IsRenderReady() const { return (material_ && mesh_); }

        s_ptr<Material> GetMaterial() const { return material_; }
        s_ptr<Mesh> GetMesh() const { return mesh_; }

		bool IsInstancingSupported(RenderPassOrder pass) const;

        size_t GetInstanceDataStride(RenderPassOrder pass) const;
        virtual void WritePerObjData(void* ptr) = 0;

    private:
        s_ptr<Transform> my_transform_ = {};

        s_ptr<Material> material_ = {};
        s_ptr<Mesh> mesh_ = {};
    };
}



