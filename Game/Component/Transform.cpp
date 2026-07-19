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
	const matrix& Transform::GetLocalMatrix()
	{
		if (is_local_dirty_) {
			matrix scale_mat = matrix::CreateScale(local_scale_);
			matrix rot_mat = matrix::CreateFromQuaternion(local_rot_);
			matrix pos_mat = matrix::CreateTranslation(local_pos_);
			local_mat_ = scale_mat * rot_mat * pos_mat;
			is_local_dirty_ = false;
		}

		return local_mat_;
	}
	const matrix& Transform::GetWorldMatrix()
	{
		if (is_world_dirty_)
		{
			if (auto current_parent = parent_)
			{
				// 내 로컬 행렬(필요시 갱신됨) * 부모의 월드 행렬(필요시 갱신됨)
				world_mat_ = GetLocalMatrix() * current_parent->GetWorldMatrix();
			}
			else
			{
				world_mat_ = GetLocalMatrix();
			}

			is_world_dirty_ = false;
		}

		return world_mat_;
	}
	void Transform::SetParent(Transform* new_parent)
	{
		if(new_parent == this) { return; }

		if(auto current_parent = parent_) {
			current_parent->RemoveChild(this);
		}

		parent_ = new_parent;
		if (new_parent)
		{
			new_parent->AddChild(this);
			SetWorldDirty();
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
}