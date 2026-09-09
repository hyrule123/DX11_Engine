#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

namespace engine
{
    class Transform;
    class Material;
    class Mesh;
    class ConstantBuffer;

    class Renderer 
        : public Component
    {
        ENTITY_INFO(Renderer, Component)
        COMPONENT_CATEGORY(ComponentCategory::kRenderer)
    public:
        Renderer(const HashedStringView& concrete_class_name);
        virtual ~Renderer() override;

        virtual void Init() override;
        virtual void Awake() override;
        virtual void LateUpdate() = 0;

		void SetRenderID(RenderPassOrder pass, uint32 id) { renderer_id[(size_t)pass] = id; }
		uint32 GetRenderID(RenderPassOrder pass) const { return renderer_id[(size_t)pass]; }

        Transform* GetTransform() const { return my_transform_; }

		size_t GetSubMeshCount() const { return materials_.size(); }

        // Mesh 먼저 설정할 것. 그래야 SubMesh 개수를 알 수 있음
        void SetMesh(s_ptr<Mesh> mesh);
        bool SetMesh(const HashedStringView& mesh_name);
        bool SetMaterial(size_t submesh_idx, s_ptr<Material> material);
        bool SetMaterial(size_t submesh_idx, const HashedStringView& mtrl_name);

        bool IsRenderReady() const { return (!materials_.empty() && mesh_); }

        s_ptr<Material> GetMaterial(size_t submesh_idx) const { return (submesh_idx < materials_.size()) ? materials_.at(submesh_idx) : nullptr; }
		const std::vector<s_ptr<Material>>& GetMaterials() const { return materials_; }

        s_ptr<Mesh> GetMesh() const { return mesh_; }

        virtual void WritePerObjectData(DataBlock data_block) = 0;
    protected:
        

    private:
		std::array<uint32, (size_t)RenderPassOrder::kEND> renderer_id = {}; // RendererManager에서 발급(Pass 별)

        Transform* my_transform_ = {};

		std::vector<s_ptr<Material>> materials_ = {};
        s_ptr<Mesh> mesh_ = {};
    };
}



