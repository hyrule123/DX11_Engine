#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>

#include <Engine/Game/Component/Component.h>

namespace engine
{
    class Transform;
    class ConstantBuffer;

    class Camera :
        public Component
    {
        ENTITY_INFO(Camera, Component)
        COMPONENT_CATEGORY(ComponentCategory::kCamera)
    public:
        enum class ProjectionMode { Orthographic, Perspective };
        struct ProjectionMatrixDesc
        {
            ProjectionMode proj_mode;
            float near_z, far_z;
            float viewport_width, viewport_height;
        };

        Camera();
        virtual ~Camera() override;

        virtual void Init() override;
        virtual void Awake() override;

        virtual void LateUpdate() override;

        const matrix& GetViewMatrix() const { return view_mat_; }

        ProjectionMatrixDesc GetProjectionMatrixDesc() const { return proj_mat_desc_; }

		float GetViewportWidth() const { return proj_mat_desc_.viewport_width; }
		float GetViewportHeight() const { return proj_mat_desc_.viewport_height; }

        void CreateProjMatrix(ProjectionMatrixDesc desc);
        const matrix& GetProjMatrix() const { return proj_mat_; }

    private:
        void CreateViewMatrix();

        Transform* my_transform_ = {};

        matrix view_mat_ = {};
        matrix proj_mat_;

        ProjectionMatrixDesc proj_mat_desc_;
    };
}


