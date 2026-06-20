#include "Engine/Core/pch.h"
#include "Transform.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/GPUBuffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/CppShared/Register.hlsli>

namespace engine
{
	Transform::Transform()
		: Super(STRINGIFY(Transform), ComponentCategory::kTransform)
		, local_scale_(float3::One)
		, local_rot_(Quaternion::Identity)
		, local_pos_(float3::Zero)
		, world_mat_(matrix::Identity)
	{
	}
	Transform::~Transform()
	{
	}
	void Transform::Init()
	{
		Super::Init();

		auto& resmgr = ResourceManager::GetInst();
		const_buffer_ = resmgr.Find<ConstantBuffer>("TransformConstBuffer");

		if (!const_buffer_)
		{
			const_buffer_ = std::make_shared<ConstantBuffer>();
			bool result = const_buffer_->Create<matrix>();
			resmgr.AddResource("TransformConstBuffer", const_buffer_);
			resmgr.SetDefaultResource(const_buffer_);
			ASSERT(result);
		}
	}

	void Transform::Awake()
	{
		Super::Awake();

		DEBUG_LOG("Transform Awake 호출!!");
	}

	void Transform::LateUpdate()
	{
		Super::LateUpdate();

		matrix scale_mat = matrix::CreateScale(local_scale_);

		matrix rot_mat = matrix::CreateFromQuaternion(local_rot_);

		matrix pos_mat = matrix::CreateTranslation(local_pos_);

		world_mat_ = scale_mat * rot_mat * pos_mat;

		UploadAndBindConstBuffer();

#define TF_DEBUG
#ifdef TF_DEBUG
		std::string msg = "LOCAL SCALE: " + std::to_string(local_scale_.x) + std::to_string(local_scale_.y) + std::to_string(local_scale_.z) + "\n";

		msg += "LOCAL ROTATION: " + std::to_string(local_rot_.x) + std::to_string(local_rot_.y) + std::to_string(local_rot_.z) + std::to_string(local_rot_.w) + "\n";

		msg += "LOCAL POS: " + std::to_string(local_pos_.x) + std::to_string(local_pos_.y) + std::to_string(local_pos_.z) + "\n\n";
		DEBUG_LOG_A(msg.c_str());
#endif
	}

	void Transform::UploadAndBindConstBuffer()
	{
		auto context = GraphicsDevice::GetInst().GetContext();
		const_buffer_->Upload(context.Get(), world_mat_);
		const_buffer_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_WORLD);
	}
}