#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>


namespace engine
{
	class ConstantBuffer;

	class Transform final 
		: public Component
	{
		CLASS_INFO(Transform, Component)
		COMPONENT_CATEGORY(ComponentCategory::kTransform)

		friend class Transform;
	public:
		Transform();
		virtual ~Transform() override;
		
		virtual void Init() override;
		virtual void Awake() override;

		virtual void LateUpdate() override;
		
		void SetLocalScale(float3 local_scale) { 
			local_scale_ = local_scale; 
			SetLocalDirty();
		}
		float3 GetLocalScale() const { return local_scale_; }

		void SetLocalRotation(Quaternion local_rot) { 
			local_rot_ = local_rot; 
			SetLocalDirty();
		}

		void SetLocalRotationEuler(float3 local_rot_euler) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(local_rot_.y, local_rot_.x, local_rot_.z);
			SetLocalDirty();
		}

		void SetLocalRotationEuler(float yaw, float pitch, float roll) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
			SetLocalDirty();
		}

		Quaternion GetLocalRotation() const { return local_rot_; }

		void SetLocalPosition(float3 local_pos) { 
			local_pos_ = local_pos; 
			SetLocalDirty();
		}
		float3 GetLocalPosition() const { return local_pos_; }

		const matrix& GetLocalMatrix();
		const matrix& GetWorldMatrix();

		void SetParent(Transform* new_parent);
		
	private:
		void AddChild(Transform* child) { if (child) { children_.push_back(child); } }

		void RemoveChild(Transform* child);

		void SetLocalDirty() {
			is_local_dirty_ = true;
			SetWorldDirty();
		}

		void SetWorldDirty() {
			if (is_world_dirty_) { return; }
			is_world_dirty_ = true;
			for (auto* child : children_) {
				child->SetWorldDirty();
			}
		}

		float3 local_scale_ = float3::One;
		Quaternion local_rot_ = Quaternion::Identity;
		float3 local_pos_ = float3::Zero;

		matrix local_mat_ = matrix::Identity;
		matrix world_mat_ = matrix::Identity;

		Transform* parent_ = {};
		std::vector<Transform*> children_ = {};

		bool is_local_dirty_ = true;
		bool is_world_dirty_ = true;
	};
}


