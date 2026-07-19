#include "Engine/Core/pch.h"
#include "Transform.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/Graphics/Buffer/ConstantBuffer.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/CppShared/Register.hlsli>

namespace engine
{
	Transform::Transform()
		: Super(STRINGIFY(Transform), ComponentCategory::kTransform)
	{
	}
	Transform::~Transform()
	{
	}

	void Transform::Init()
	{
		Super::Init();
	}

	void Transform::Awake()
	{
		Super::Awake();

		DEBUG_LOG("Transform Awake 호출!!");
	}

	void Transform::LateUpdate()
	{
		Super::LateUpdate();

		GetWorldMatrix();
	}
	void Transform::SetWorldScale(float3 world_scale)
	{
		if (parent_)
		{
			float3 parent_world_scale = parent_->GetWorldScale();
			local_scale_ = world_scale / parent_world_scale;
		}
		else
		{
			local_scale_ = world_scale;
		}
		SetDirty();
	}
	void Transform::SetWorldRotation(Quaternion world_rot)
	{
		if (parent_)
		{
			Quaternion parent_world_rot_inv;
			parent_->GetWorldRotation().Inverse(parent_world_rot_inv);
			local_rot_ = world_rot * parent_world_rot_inv;
		}
		else
		{
			local_rot_ = world_rot;
		}
		SetDirty();
	}
	void Transform::SetWorldPosition(float3 world_pos)
	{
		if (parent_)
		{
			parent_->UpdateWorldTransform();

			float3 scaled_rot_local_pos = world_pos - parent_->world_pos_;

			Quaternion parent_world_rot_inv;
			parent_->world_rot_.Inverse(parent_world_rot_inv);

			float3 scaled_pos = float3::Transform(scaled_rot_local_pos, parent_world_rot_inv);

			local_pos_ = scaled_pos / parent_->world_scale_;
		}
		else
		{
			local_pos_ = world_pos;
		}
		SetDirty();
	}
	void Transform::SetParent(Transform* new_parent)
	{
		if(new_parent == this) { return; }

		if(parent_) {
			parent_->RemoveChild(this);
		}

		parent_ = new_parent;
		if (new_parent)
		{
			new_parent->AddChild(this);
			SetDirty();
		}
	}
	void Transform::RemoveChild(Transform* child)
	{
		if (!child) { return; }

		auto iter = children_.begin();
		for (iter; iter != children_.end(); ++iter) {
			if (*iter == child) {
				children_.erase(iter);
				break;
			}
		}
	}
	void Transform::UpdateWorldTransform()
	{
		if (!is_dirty_) { return; }

		if (parent_)
		{
			parent_->UpdateWorldTransform();

			world_scale_ = local_scale_ * parent_->world_scale_;

			//Q1 * Q2 -> Q1 회전 후 Q2 회전(Local -> World)
			world_rot_ = local_rot_ * parent_->world_rot_;

			// 부모 포함 모든 Scale이 적용된 위치
			float3 scaled_local_pos = local_pos_ * parent_->world_scale_;

			// Scale 적용 후 부모의 월드 회전값을 활용하여 회전
			float3 rotated_pos = float3::Transform(scaled_local_pos, parent_->world_rot_);

			world_pos_ = parent_->world_pos_ + rotated_pos;
		}
		else
		{
			world_scale_ = local_scale_;
			world_rot_ = local_rot_;
			world_pos_ = local_pos_;
		}

		world_mat_ = matrix::CreateScale(world_scale_) *
			matrix::CreateFromQuaternion(world_rot_) *
			matrix::CreateTranslation(world_pos_);

		is_dirty_ = false;
	}
}