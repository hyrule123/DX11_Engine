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
	public:
		Transform();
		virtual ~Transform() override;
		
		virtual void Init() override;
		virtual void Awake() override;

		virtual void LateUpdate() override;
		
		void SetLocalScale(float3 local_scale) { local_scale_ = local_scale; }
		float3 GetLocalScale() const { return local_scale_; }

		void SetLocalRotation(Quaternion local_rot) { local_rot_ = local_rot; }

		void SetLocalRotationEuler(float3 local_rot_euler) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(local_rot_.y, local_rot_.x, local_rot_.z);
		}

		void SetLocalRotationEuler(float yaw, float pitch, float roll) {
			local_rot_ = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
		}

		Quaternion GetLocalRotation() const { return local_rot_; }

		void SetLocalPosition(float3 local_pos) { local_pos_ = local_pos; }
		float3 GetLocalPosition() const { return local_pos_; }

		const matrix& GetWorldMatrix() const { return world_mat_; }
		
	private:
		float3 local_scale_;
		Quaternion local_rot_;
		float3 local_pos_;

		matrix world_mat_;
	};
}


