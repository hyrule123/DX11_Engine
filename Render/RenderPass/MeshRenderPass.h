#pragma once

#include <Engine/Render/RenderPass/RenderPass.h>
#include <Engine/Render/RenderTypes.h>	// RenderKey

#include <vector>

namespace engine
{
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
		void Push(RenderKey key, uint32 renderer_id, float depth);

		virtual void Execute(const RenderContext& ctx) final;

		SortPolicy GetSortPolicy() const { return sort_policy_; }

	protected:
		// 패스당 1회. 기본은 ctx.view의 출력 대상 바인딩.
		// 전용 대상(그림자 DSV, ROV UAV 등)을 쓰는 패스만 오버라이드하며, 그 대상의 클리어도 여기서 처리한다.
		virtual void BindTargets(const RenderContext& ctx);

	private:
		struct QueueItem
		{
			uint64 sort_key;
			RenderKey key;
			uint32 renderer_id;
		};

		void Sort();
		void DrawQueue(const RenderContext& ctx);

		std::vector<QueueItem> queue_ = {};
		const SortPolicy sort_policy_;
	};
}