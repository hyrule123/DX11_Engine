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
        CLASS_INFO(Camera, Component)
    public:
        Camera();
        virtual ~Camera() override;

        virtual void Init() override;
        virtual void Awake() override;

        virtual void LateUpdate() override;

        const matrix& GetViewMatrix() const { return view_mat_; }
        const matrix& GetProjMatrix() const { return proj_mat_; }

    private:
        void CreateViewMatrix();
        void CreateProjMatrix(float width, float height);

        s_ptr<Transform> my_transform_ = {};

        matrix view_mat_ = {};
        matrix proj_mat_ = {};
    };
}


