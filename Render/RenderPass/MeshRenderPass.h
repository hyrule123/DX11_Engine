#pragma once

#include <Engine/Render/RenderPass/RenderPass.h>
#include <Engine/Render/RenderTypes.h>	// RenderKey

#include <vector>

namespace engine
{
	class Renderer;
	class RenderTarget;

	// Renderer를 서브메시 단위로 그리는 패스의 공통 구현.
	// - 항목을 캐싱하지 않는다. RenderManager가 매 프레임(View마다) Clear() 후
	//   컬링 통과분을 전개하며 Push()로 채운다.
	// - 흐름: Sort → BindTargets → DrawQueue
	class MeshRenderPass : public RenderPass
	{
		ENTITY_INFO(MeshRenderPass, RenderPass);
	public:
		enum class SortPolicy : uint8
		{
			State,			// sort_key = RenderKey. 배칭 최대화 (불투명, 알파 테스트)
			FrontToBack,	// 깊이 오름차순 (early-Z)
			BackToFront,	// 깊이 내림차순 (반투명, 페인터 순서)
		};

		MeshRenderPass(render_pass::ID id, SortPolicy sort_policy);
		virtual ~MeshRenderPass() override;

		void Clear() { queue_.clear(); }
		void Reserve(size_t count) { queue_.reserve(count); }

		// depth: RenderView::ComputeViewDepth 결과. State 정책에서는 사용하지 않는다.
		void Push(RenderKey key, uint32 renderer_id, float depth, uint32 obj_data_stride);

		virtual void Execute(const RenderContext& ctx) final;

		SortPolicy GetSortPolicy() const { return sort_policy_; }

		void SetOutputTarget(std::shared_ptr<RenderTargetGroup> rtg, const ClearSettings& clear_settings)
		{
			rtg_override_ = rtg;
			rtg_override_clear_setting_ = clear_settings;
		}

	protected:
		// 패스당 1회. 기본은 ctx.view의 출력 대상 바인딩.
		// 전용 대상(그림자 DSV, ROV UAV 등)을 쓰는 패스만 오버라이드하며, 그 대상의 클리어도 여기서 처리한다.
		virtual void BindTargets(const RenderContext& ctx);

	private:
		void Sort();
		void BuildGroups(const RenderContext& ctx);	// 사전 Buffer 용량 계산
		void UploadInstances(const RenderContext& ctx); // 사전 계산된 Buffer에 데이터 채워넣기 & Draw Command 생성
		void DrawGroups(const RenderContext& ctx);

	private:
		struct QueueItem
		{
			uint64 sort_key;
			RenderKey key;
			uint32 renderer_id;
			uint32 obj_data_stride;
		};

		struct DrawGroup
		{
			uint32 queue_begin = 0;							// queue_ 내 시작 인덱스
			uint32 instance_count = 0;						// 그룹 인스턴스 수 (stride 0이어도 유효)
			InstanceDataAllocation alloc = {};		// stride 0이면 무효
			Renderer* renderer = nullptr;					// 그룹 첫 Renderer (상태 해석용)
		};

	private:
		const SortPolicy sort_policy_;
		std::vector<QueueItem> queue_;
		std::vector<DrawGroup> draw_groups_;
		std::shared_ptr<RenderTargetGroup> rtg_override_;
		ClearSettings rtg_override_clear_setting_;
	};
}