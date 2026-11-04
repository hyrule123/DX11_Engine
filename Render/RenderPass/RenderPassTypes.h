#pragma once
#include <Engine/Core/CoreMinimal.h>

#include <Engine/Util/EnumFlags.h>

namespace engine
{
	class Camera;
	class RenderTargetGroup;

	// RenderTargetGroup의 Clear API가 Depth/Stencil을 함께 지우므로 둘을 하나로 묶는다.
	enum class ClearTarget : uint8_t
	{
		Color = 0,
		DepthStencil,
		kCount
	};
	ENABLE_ENUM_FLAGS(ClearTargetFlags, ClearTarget, uint8_t);

	struct ClearSettings
	{
		ClearTargetFlags flags = ClearTarget::Color | ClearTarget::DepthStencil;
		std::array<float, 4> color = { 0.0f, 0.0f, 0.0f, 0.0f };
		float depth = 1.0f;
		uint8_t stencil = 0;
	};

	namespace render_pass
	{
		enum class ID : uint32_t
		{
			// 내장 패스
			ForwardOpaque = 0, ForwardBlend, Present,

			// 커스텀 예약 슬롯 (콘텐츠에서 이름을 붙여 사용)
			Custom0, Custom1, Custom2, Custom3, Custom4, Custom5, Custom6, Custom7,
			Custom8,

			kCount
		};

		static_assert((size_t)ID::kCount < 32, "render_pass::ID can't exceed 32");

		ENABLE_ENUM_FLAGS(Flags, ID, uint32_t);
	}

	// 함수에 넘길 때는 const &로 전달할 것(vector 복사 방지)
	struct RenderViewDesc
	{
		weak_handle_ptr<Camera> camera = {};			// 카메라가 비어 있으면 컬링과 전개 없이 패스만 실행
		std::shared_ptr<RenderTargetGroup> rtg = {};	// nullptr을 전달 시 SwapChain RTG를 사용
		ClearSettings clear_settings = {};				// 기본값 있음, 선언 확인
		std::vector<render_pass::ID> pass_order = {};	// 비어 있으면 RenderManager의 기본 패스 순서를 사용
	};
}