#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/Math.h>
#include <Engine/Core/CoreMinimal.h>

namespace engine
{
	class Transform final 
		: public Component
	{
		CLASS_INFO(Transform, Component)
		COMPONENT_CATEGORY(ComponentCategory::kTransform)
	public:
		Transform();
		virtual ~Transform() override;
		
		virtual void Init() override;
		virtual void Awake() override;

		virtual void LateUpdate() override;
		
		inline void SetLocalScale(const float3& local_scale) { local_scale_ = local_scale; }
		inline const float3& GetLocalScale() const { return local_scale_; }

		inline void SetLocalRotation(const Quaternion& local_rot) { local_rot_ = local_rot; }

		inline void SetLocalRotationEuler(const float3& local_rot_euler) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(local_rot_.y, local_rot_.x, local_rot_.z);
		}

		inline void SetLocalRotationEuler(float yaw, float pitch, float roll) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
		}

		inline const Quaternion& GetLocalRotation() const { return local_rot_; }

		inline void SetLocalPosition(const float3& local_pos) { local_pos_ = local_pos; }
		inline const float3& GetLocalPosition() const { return local_pos_; }

		inline const MATRIX& GetWorldMatrix() const { return world_mat_; }
		
	private:
		float3 local_scale_;
		Quaternion local_rot_;
		float3 local_pos_;

		MATRIX world_mat_;
	};
}


