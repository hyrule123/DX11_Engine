#include "Engine/Core/pch.h"
#include "Camera.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/GPU/Buffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/Collision/Geometry2D.h>

#include <Engine/HLSL/Core/Register.hlsli>

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

		Camera* cam = RenderManager::GetInst().GetMainCamera().get();
		if (nullptr == cam)
		{
			wh_ptr<Camera> whptr = static_handle_cast<Camera>(weak_handle_from_this());
			RenderManager::GetInst().SetMainCamera(whptr);
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
		if (desc.proj_mode == ProjectionMode::Perspective)
		{
			proj_mat_ = matrix::CreatePerspectiveLH(desc.viewport_width, desc.viewport_height, desc.near_z, desc.far_z);
		}
		else if (desc.proj_mode == ProjectionMode::Orthographic)
		{
			proj_mat_ = matrix::CreateOrthographicLH(desc.viewport_width, desc.viewport_height, desc.near_z, desc.far_z);
		}
		proj_mat_desc_ = desc;
	}

	AABB2D Camera::ComputeViewBounds2D() const
	{
		ASSERT_MESSAGE(proj_mat_desc_.proj_mode == ProjectionMode::Orthographic, "Ortho 카메라만 호출하세요.");

		constexpr float kMarginRatio = 0.505f; // 약간의 여유를 두어 한두픽셀 잘리는 것을 방지
		const float half_width = proj_mat_desc_.viewport_width * kMarginRatio;
		const float half_height = proj_mat_desc_.viewport_height * kMarginRatio;

		const AABB3D local_bounds = {
			{-half_width, -half_height, proj_mat_desc_.near_z},
			{half_width, half_height, proj_mat_desc_.near_z}
		};

		const matrix world_mat = my_transform_->GetWorldMatrix();
		
		return geometry_2d::TransformBoundsTo2D(local_bounds, world_mat);
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

