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
		: Super(Transform::kClassConcreteName, ComponentCategory::kTransform)
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
	void Transform::OnDestroy()
	{
		Super::OnDestroy();

		if (parent_ && !(parent_->IsDestroyed()))
		{
			SetParent(nullptr);
		}

		// 자식 배열은 어차피 재귀 구조로 모두 Destroy 마킹이 되었으므로 필요 없음
		// Destroy 되면 AddChild도 막히므로 Destroy 되지 않은 Transform이 들어올 수 없음.
	}
	matrix Transform::GetWorldMatrix()
	{
		UpdateWorldTransform();

		return DirectX::XMMatrixAffineTransformation(
			GetWorldScale(),
			DirectX::g_XMZero, // <-- float3(0,0,0) 대신 DirectX 내장 Zero 상수 사용 ( XMLoad 과정 생략 )
			GetWorldRotation(),
			GetWorldPosition()
		);
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
			// '자전' 후 '부모 축 기준'으로 다시 회전
			world_rot_ = local_rot_ * parent_->world_rot_;

			// 부모의 'Scale'을 적용한 'Position'
			float3 scaled_local_pos = local_pos_ * parent_->world_scale_;

			// 부모의 'Rotation'을 적용한 'Position' (= 공전)
			float3 rotated_pos = float3::Transform(scaled_local_pos, parent_->world_rot_);

			// 공전된 Position을 world_pos에 더해준다
			world_pos_ = parent_->world_pos_ + rotated_pos;
		}
		else
		{
			world_scale_ = local_scale_;
			world_rot_ = local_rot_;
			world_pos_ = local_pos_;
		}

		is_dirty_ = false;
		//버전을 한 단계 올림
		++version_;
	}
}