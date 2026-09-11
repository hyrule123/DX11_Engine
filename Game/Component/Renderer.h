#pragma once

#include <Engine/Game/Component/Component.h>

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

        virtual void Awake() override;
        virtual void OnEnable() override;
        virtual void OnDisable() override;

		void SetRenderSlot(RenderPassOrder pass, uint32 id) { renderer_slots_[(size_t)pass] = id; }
		uint32 GetRenderSlot(RenderPassOrder pass) const { return renderer_slots_[(size_t)pass]; }
		const std::array<uint32, (size_t)RenderPassOrder::kEND>& GetRenderSlots() const { return renderer_slots_; }
		void ClearRenderSlots() { renderer_slots_.fill(kInvalidIdx32); }

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

        AABB2D ComputeWorldBounds2D() const;
    protected:
        

    private:
		std::array<uint32, (size_t)RenderPassOrder::kEND> renderer_slots_; // RendererManager에서 발급(Pass 별), MAX로 초기화

        Transform* my_transform_ = {};

		std::vector<s_ptr<Material>> materials_ = {};
        s_ptr<Mesh> mesh_ = {};
    };
}



