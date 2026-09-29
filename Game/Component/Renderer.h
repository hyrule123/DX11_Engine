#pragma once

#include <Engine/Game/Component/Component.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Render/RenderTypes.h>


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

        // RenderSlot Invalid == kInvalidIdx32
        void SetRenderSlot(RenderPassOrder pass, uint32 id) { ASSERT(id != kInvalidIdx32); renderer_slots_[(size_t)pass] = id; }
		void InvalidateRenderSlot(RenderPassOrder pass) { renderer_slots_[(size_t)pass] = kInvalidIdx32; }
		uint32 GetRenderSlot(RenderPassOrder pass) const { return renderer_slots_[(size_t)pass]; }
		bool IsValidRenderSlot(RenderPassOrder pass) const { return (renderer_slots_[(size_t)pass] != kInvalidIdx32); }
		const std::array<uint32, (size_t)RenderPassOrder::kCount>& GetRenderSlots() const { return renderer_slots_; }
		void ClearRenderSlots() { renderer_slots_.fill(kInvalidIdx32); }

        Transform* GetTransform() const { return my_transform_; }

		size_t GetSubMeshCount() const { return materials_.size(); }

        // Mesh 먼저 설정할 것. 그래야 SubMesh 개수를 알 수 있음
        void SetMesh(std::shared_ptr<Mesh> mesh);
        bool SetMesh(const HashedStringView& mesh_name);
        bool SetMaterial(size_t submesh_idx, std::shared_ptr<Material> material);
        bool SetMaterial(size_t submesh_idx, const HashedStringView& mtrl_name);

        bool IsRenderReady() const { return (!materials_.empty() && mesh_); }

        Material* GetMaterial(size_t submesh_idx) const { return (submesh_idx < materials_.size()) ? materials_.at(submesh_idx).get() : nullptr; }
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return materials_; }

        Mesh* GetMesh() const { return mesh_.get(); }

        virtual void WritePerObjectData(DataBlock data_block) = 0;

        AABB2D ComputeWorldBounds2D() const;

		SubMeshRenderData GetSubMeshRenderData(size_t submesh_idx) const {
			ASSERT(submesh_idx < submesh_render_data_.size());
			return submesh_render_data_[submesh_idx];
		}
		const std::vector<SubMeshRenderData>& GetAllSubMeshRenderData() const { return submesh_render_data_; }

		RenderPassFlags GetRenderPassFlags() const {
            RenderPassFlags flags = {};
			for (const auto& submesh_data : submesh_render_data_) {
				flags |= submesh_data.pass_flags;
			}
			return flags;
		}

    private:
        void BuildSubMeshRenderData();

    private:
        Transform* my_transform_ = {};

		std::vector<std::shared_ptr<Material>> materials_ = {};
        std::shared_ptr<Mesh> mesh_ = {};

        std::array<uint32, (size_t)RenderPassOrder::kCount> renderer_slots_; // RendererManager에서 발급(Pass 별), MAX로 초기화
        std::vector<SubMeshRenderData> submesh_render_data_;
    };
}



