#pragma once
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Math.h>
#include <Engine/Core/Enum.h>

#include <Engine/Collision/Collision.h>	// AABB2D

#include <Engine/Render/RenderTypes.h>

#include <Engine/HLSL/Camera.hlsli>

#include <array>

struct ID3D11DeviceContext;

namespace engine
{
	class Camera;
	class RenderTargetGroup;

	// 한 프레임 동안 "어디서 보고, 어디에 그리는가"를 담는 값 타입.
	// - RenderManager가 매 프레임 Camera로부터 Setup()으로 다시 채운다.
	// - 출력 대상(RenderTargetGroup)은 소유하지 않는다. 소유는 RenderManager(또는 씬 렌더 설정)가 하고,
	//   이 객체는 프레임 동안만 유효한 비소유 포인터를 들고 있다.
	// - 뷰포트는 RenderTargetGroup::BindOutputMerger가 함께 설정하므로 따로 두지 않는다.
	class RenderView
	{
	public:
		RenderView() = default;

		void Setup(const Camera& camera, RenderTargetGroup* target, const ClearSettings& clear_settings);

		// 패스가 기본 출력 대상을 바인딩할 때 사용 (MeshRenderPass::BindTargets 기본 구현)
		void BindTargets(ID3D11DeviceContext* context) const;

		// View의 첫 패스 실행 전에 RenderManager가 1회 호출
		void ClearTargets(ID3D11DeviceContext* context) const;

		// View 공간 깊이: 카메라 위치에서 전방 벡터 방향으로의 거리 (sort_key용)
		float ComputeViewDepth(const float3& world_pos) const
		{
			return (world_pos - position_).Dot(forward_);
		}

		CameraData BuildCameraData() const
		{
			CameraData data = {};
			data.view_mat = view_mat_;
			data.proj_mat = proj_mat_;
			data.view_proj = view_proj_mat_;
			return data;
		}

		const matrix& GetViewMatrix() const { return view_mat_; }
		const matrix& GetProjMatrix() const { return proj_mat_; }
		const matrix& GetViewProjMatrix() const { return view_proj_mat_; }

		const float3& GetPosition() const { return position_; }
		const float3& GetForward() const { return forward_; }

		// 직교 카메라일 때만 유효. 원근 카메라면 무효(min > max) 상태로 남는다.
		const AABB2D& GetViewBounds2D() const { return view_bounds_2d_; }
		bool IsOrthographic() const { return is_orthographic_; }

		RenderTargetGroup* GetTarget() const { return target_; }

	private:
		matrix view_mat_ = {};
		matrix proj_mat_ = {};
		matrix view_proj_mat_ = {};

		float3 position_ = {};
		float3 forward_ = {};

		AABB2D view_bounds_2d_ = {};
		bool is_orthographic_ = false;

		RenderTargetGroup* target_ = nullptr;
		ClearSettings clear_settings_ = {};
	};
}