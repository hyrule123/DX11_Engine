#pragma once

#include <Engine/Render/RenderPass/RenderPass.h>

#include <Engine/Render/RenderTypes.h>

#include <memory>

namespace engine
{
	class Mesh;
	class PipelineState;
	class PixelShader;
	class Texture2D;
	class RenderTargetGroup;

	// Blit = Bit Block Transfer(BitBLT)를 줄인 말. Unity나 Windows GDI에서도 사용함
	// 
	// 입력 텍스처 하나를 전체 화면 쿼드로 출력 대상에 그리는 범용 패스 (셰이더를 낀 blit).
	// - 입력: 지정한 텍스처. 지정하지 않으면 View 타깃(RTG의 SRV 전체를 REG_T_SRC_RENDERTARGET부터 바인딩).
	// - 출력: View 타깃(기본) / 스왑체인 / 지정한 RTG. 뷰포트는 출력 RTG의 것을 그대로 사용.
	// - PS: 지정하지 않으면 기본 복사 PS. 입력은 REG_T_SRC_RENDERTARGET 슬롯으로 들어간다.
	// - 깊이 테스트/쓰기 없음.
	// 예) Present = BlitPass(ID::Present) + SetOutputSwapChain()
	//     팔레트 합성 = BlitPass(Custom) + SetInputTexture(인덱스 RT) + SetPixelShader(팔레트 PS)
	class BlitPass
		: public RenderPass
	{
		ENTITY_INFO(BlitPass, RenderPass);
	public:
		explicit BlitPass(render_pass::ID id);
		virtual ~BlitPass() override;

		virtual void Init() override;
		virtual void Execute(const RenderContext& ctx) override;

		// nullptr이면 View 타깃을 입력으로 사용
		void SetInputTexture(std::shared_ptr<Texture2D> texture);

		// nullptr이면 View 타깃에 출력. clear의 flags가 비어 있으면 클리어하지 않음
		void SetOutputTarget(std::shared_ptr<RenderTargetGroup> rtg, const ClearSettings& clear);

		// 스왑체인에 출력 (스왑체인 RTG는 GraphicsDevice가 소유하므로 별도 함수)
		void SetOutputSwapChain(const ClearSettings& clear);

		// nullptr이면 기본 복사 PS
		void SetPixelShader(std::shared_ptr<PixelShader> pixel_shader);

	protected:
		// 파생 클래스용 (ENTITY_INFO 이름 전달)
		BlitPass(const HashedStringView& concrete_class_name, render_pass::ID id);

	private:
		enum class OutputMode : uint8
		{
			ViewTarget,
			SwapChain,
			RenderTargetGroup,
		};

		RenderTargetGroup* ResolveOutput(const RenderContext& ctx) const;

	private:
		std::unique_ptr<Mesh> mesh_;
		std::unique_ptr<PipelineState> pipeline_state_;
		std::shared_ptr<PixelShader> default_pixel_shader_;

		std::shared_ptr<Texture2D> input_texture_;

		OutputMode output_mode_;
		std::shared_ptr<RenderTargetGroup> output_target_;
		ClearSettings output_clear_;
	};
}