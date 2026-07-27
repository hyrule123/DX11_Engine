#include "Engine/Core/pch.h"
#include "Camera.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/Graphics/Buffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/CppShared/Register.hlsli>

namespace engine
{
	Camera::Camera()
		: Super(Camera::kClassConcreteName, ComponentCategory::kCamera)
		, proj_mat_(matrix::Identity)
	{
		proj_mat_desc_.proj_mode = ProjectionMode::Orthographic;
		proj_mat_desc_.near_z = 1.0f;
		proj_mat_desc_.far_z = 100.0f;
	}

	Camera::~Camera()
	{}

	void Camera::Init()
	{
		Super::Init();

		if (!RenderManager::GetInst().GetMainCamera())
		{
			s_ptr<Camera> ths = std::static_pointer_cast<Camera>(shared_from_this());
			RenderManager::GetInst().SetMainCamera(ths);
		}
	}
	void Camera::Awake()
	{
		Super::Awake();
		my_transform_ = GetComponent<Transform>();
		ASSERT(my_transform_);

		//Awake 시점까지 projection matrix 미생성 시 자동 지정
		if (proj_mat_ == matrix::Identity)
		{
			proj_mat_desc_.viewport_width = (float)(GraphicsDevice::GetInst().GetResolutionWidth());
			proj_mat_desc_.viewport_height = (float)(GraphicsDevice::GetInst().GetResolutionHeight());
			CreateProjMatrix(proj_mat_desc_);
		}
	}
	void Camera::LateUpdate()
	{
		Super::LateUpdate();

		CreateViewMatrix();
	}

	void Camera::CreateProjMatrix(ProjectionMatrixDesc desc)
	{
		//이론 정리
		//https://app.notion.com/p/hyrule1/3D-Graphics-Study-250cb63f18c18074b5dcca4609f4b447
		//+ Reversed Z 적용
		if (desc.proj_mode == ProjectionMode::Perspective)
		{
			proj_mat_ = matrix::CreatePerspectiveLH(desc.viewport_width, desc.viewport_height, desc.far_z, desc.near_z);
		}
		else if (desc.proj_mode == ProjectionMode::Orthographic)
		{
			proj_mat_ = matrix::CreateOrthographicLH(desc.viewport_width, desc.viewport_height, desc.far_z, desc.near_z);
		}
		proj_mat_desc_ = desc;
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

		view_mat_ = matrix::CreateTranslation(inverse_pos) * matrix::CreateFromQuaternion(inverse_rot);
	}
}

