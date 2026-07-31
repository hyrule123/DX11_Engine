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

		virtual void OnDestroy() override;

		float3 GetLocalScale() const { return local_scale_; }
		Quaternion GetLocalRotation() const { return local_rot_; }
		float3 GetLocalPosition() const { return local_pos_; }

		void SetLocalScale(float3 local_scale) {
			local_scale_ = local_scale;
			SetDirty();
		}
		void SetLocalRotation(Quaternion local_rot) { 
			local_rot_ = local_rot; 
			SetDirty();
		}
		void SetLocalRotationEuler(float3 local_rot_euler) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(local_rot_.y, local_rot_.x, local_rot_.z);
			SetDirty();
		}
		void SetLocalRotationEuler(float yaw, float pitch, float roll) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
			SetDirty();
		}
		void SetLocalPosition(float3 local_pos) { 
			local_pos_ = local_pos; 
			SetDirty();
		}

		float3 GetWorldScale() {
			UpdateWorldTransform();
			return world_scale_;
		}
		Quaternion GetWorldRotation() {
			UpdateWorldTransform();
			return world_rot_;
		}
		float3 GetWorldPosition() {
			UpdateWorldTransform();
			return world_pos_;
		}

		//어차피 최종 렌더링 시에만 호출하므로, 매번 호출 시점에 계산하도록 함. (캐싱 X)
		matrix GetWorldMatrix();

		void SetWorldScale(float3 world_scale);
		void SetWorldRotation(Quaternion world_rot);
		void SetWorldRotationEuler(float3 world_rot_euler) {
			SetWorldRotation(
				Quaternion::CreateFromYawPitchRoll(world_rot_euler.y, world_rot_euler.x, world_rot_euler.z));
		}
		void SetWorldRotationEuler(float yaw, float pitch, float roll) {
			SetWorldRotation(Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll));
		}
		void SetWorldPosition(float3 world_pos);

		void SetParent(Transform* new_parent);
		Transform* GetParent() const { return parent_; }
		const std::vector<Transform*>& GetChildren() const { return children_; }

		uint32 GetVersion() { 
			if (is_dirty_) { UpdateWorldTransform(); }
			return version_;
		}
		
	private:
		void AddChild(Transform* child) { 
			if (!IsDestroyed() && child && !child->IsDestroyed()) { 
				children_.push_back(child); 
			} }
		void RemoveChild(Transform* child);

		void SetDirty() {
			if (is_dirty_) { return; }
			is_dirty_ = true;
			for (auto* child : children_) {
				child->SetDirty();
			}
		}

		void UpdateWorldTransform();

		// 1. Local SRT
		float3 local_scale_ = float3::One;
		Quaternion local_rot_ = Quaternion::Identity;
		float3 local_pos_ = float3::Zero;

		// 2. Cached World SRT
		float3 world_scale_ = float3::One;
		Quaternion world_rot_ = Quaternion::Identity;
		float3 world_pos_ = float3::Zero;

		Transform* parent_ = {};
		std::vector<Transform*> children_ = {};

		uint32 version_ = 0; //변경될 때마다 증가.
		bool is_dirty_ = true;
	};
}


