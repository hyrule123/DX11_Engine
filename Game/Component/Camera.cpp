#include "Engine/Core/pch.h"
#include "Camera.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/GPUBuffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/CppShared/Register.hlsli>

namespace engine
{
	Camera::Camera()
		: Super(STRINGIFY(Camera), ComponentCategory::kCamera)
	{
	}

	Camera::~Camera()
	{}

	void Camera::Init()
	{
		Super::Init();

		cam_constant_buffer_ = ResourceManager::GetInst().Find<ConstantBuffer>("CameraConstantBuffer");
		if (!cam_constant_buffer_)
		{
			CreateCameraConstantBuffer();
		}
	}
	void Camera::Awake()
	{
		Super::Awake();
		my_transform_ = GetComponent<Transform>();
		ASSERT(my_transform_);
	}
	void Camera::LateUpdate()
	{
		Super::LateUpdate();

		CreateViewMatrix();

		float width = (float)GraphicsDevice::GetInst().GetResolutionWidth();
		float height = (float)GraphicsDevice::GetInst().GetResolutionHeight();
		CreateProjMatrix(width, height);
		
		auto context = GraphicsDevice::GetInst().GetContext();

		UploadToBuffer(context.Get());
		BindToBuffer(context.Get());
	}
	void Camera::UploadToBuffer(ID3D11DeviceContext* context)
	{
		cam_constant_buffer_->Upload(context, cam_data_);
	}
	void Camera::BindToBuffer(ID3D11DeviceContext* context)
	{
		cam_constant_buffer_->Bind(context, ShaderStage::kAllGraphics, SLOT_B_CAMERA);
	}
	void Camera::CreateCameraConstantBuffer()
	{
		cam_constant_buffer_ = std::make_shared<ConstantBuffer>();
		ResourceManager::GetInst().AddResource("CameraConstantBuffer", cam_constant_buffer_);
		ResourceManager::GetInst().SetDefaultResource(cam_constant_buffer_);

		bool result = cam_constant_buffer_->Create<CameraData>();
		ASSERT(result);
	}
	void Camera::CreateViewMatrix()
	{
		//World = S * R * T
		//View = T^-1 * R^-1, Scale는 보통 View행렬에서 취급 안함

		//-를 곱해서 반대로 이동
		float3 inverse_pos = my_transform_->GetLocalPosition();
		inverse_pos *= -1.0f;

		//켤레 쿼터니언을 사용하여 반대로 회전
		Quaternion inverse_rot = my_transform_->GetLocalRotation();
		inverse_rot.Conjugate();

		cam_data_.view_mat = matrix::CreateTranslation(inverse_pos) * matrix::CreateFromQuaternion(inverse_rot);
	}
	void Camera::CreateProjMatrix(float width, float height)
	{
		cam_data_.proj_mat = matrix::CreatePerspectiveLH(width, height, 1.0f, 100.0f);
	}
}

