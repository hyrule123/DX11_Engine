#include "Engine/Core/pch.h"
#include "Transform.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	Transform::Transform()
		: Super(STRINGIFY(Transform), ComponentCategory::kTransform)
		, local_scale_(float3::One)
		, local_rot_(Quaternion::Identity)
		, local_pos_(float3::Zero)
		, world_mat_(MATRIX::Identity)
	{
	}
	Transform::~Transform()
	{
	}
	void Transform::Init()
	{
		Super::Init();

		DEBUG_LOG("Transform Init 호출!!");
	}

	void Transform::Awake()
	{
		Super::Awake();

		DEBUG_LOG("Transform Awake 호출!!");
	}

	void Transform::LateUpdate()
	{
		Super::LateUpdate();

		MATRIX scale_mat = MATRIX::CreateScale(local_scale_);

		MATRIX rot_mat = MATRIX::CreateFromQuaternion(local_rot_);

		MATRIX pos_mat = MATRIX::CreateTranslation(local_pos_);

		world_mat_ = scale_mat * rot_mat * pos_mat;

//#define TF_DEBUG
#ifdef TF_DEBUG
		std::string msg = "LOCAL SCALE: " + std::to_string(local_scale_.x) + std::to_string(local_scale_.y) + std::to_string(local_scale_.z) + "\n";

		msg += "LOCAL ROTATION: " + std::to_string(local_rot_.x) + std::to_string(local_rot_.y) + std::to_string(local_rot_.z) + std::to_string(local_rot_.w) + "\n";

		msg += "LOCAL POS: " + std::to_string(local_pos_.x) + std::to_string(local_pos_.y) + std::to_string(local_pos_.z) + "\n\n";
		DEBUG_LOG_A(msg.c_str());
#endif
	}


}