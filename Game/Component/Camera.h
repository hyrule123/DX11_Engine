#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>

#include <Engine/Game/Component/Component.h>

#include <Engine/HLSL/CppShared/Struct.hlsli>

struct ID3D11DeviceContext;

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

    private:
        void UploadToBuffer(ID3D11DeviceContext* context);
        void BindToBuffer(ID3D11DeviceContext* context);
        void CreateCameraConstantBuffer();
        void CreateViewMatrix();
        void CreateProjMatrix(float width, float height);

        s_ptr<Transform> my_transform_ = {};

        //View, Proj
        CameraData cam_data_ = {};

        s_ptr<ConstantBuffer> cam_constant_buffer_ = {};
    };
}


