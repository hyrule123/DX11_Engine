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

        virtual void OnTransformDirty(Transform* transform) override;

		uint32_t GetRendererId() const { return renderer_id_; }
		void SetRendererId(uint32_t id) { renderer_id_ = id; }
        bool IsRegistered() const { return renderer_id_ != kInvalidIdx32; }

        Transform* GetTransform() const { return my_transform_; }

		// Mesh 웬만하면 먼저 설정할 것. 그래야 SubMesh 개수를 알 수 있음(Material 설정 시 submesh_idx 범위 체크에서 통과 못할수도 있음)
        void SetMesh(std::shared_ptr<Mesh> mesh);
        bool SetMesh(const HashedStringView& mesh_name);
        bool SetMaterial(size_t submesh_idx, std::shared_ptr<Material> material);
        bool SetMaterial(size_t submesh_idx, const HashedStringView& mtrl_name);

        bool IsRenderReady() const { return (!submeshes_.empty() && mesh_); }

        size_t GetSubMeshCount() const { return submeshes_.size(); }
		const SubMesh& GetSubMesh(size_t submesh_idx) const { return submeshes_.at(submesh_idx); }
		const std::vector<SubMesh>& GetSubMeshes() const { return submeshes_; }
		Material* GetMaterial(size_t submesh_idx) const { return submeshes_.at(submesh_idx).material.get(); }

        Mesh* GetMesh() const { return mesh_.get(); }

        virtual void WriteInstanceData(DataBlock data_block) = 0;

        AABB3D ComputeWorldBounds() const;

        void SetLocalBoundsOverride(const AABB3D& local_bounds);
        void ClearLocalBoundsOverride() { SetLocalBoundsOverride(AABB3D::Invalid()); }

    private:
        void BuildSubMeshRenderData(size_t submesh_idx);

    private:
        std::shared_ptr<Mesh> mesh_ = {};
        std::vector<SubMesh> submeshes_ = {};

        uint32_t renderer_id_; // RenderManager에서 발급

        Transform* my_transform_ = {};

		AABB3D local_bounds_override_ = {};
    };
}



