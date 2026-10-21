#pragma once

#include <Engine/Render/RenderPass/RenderPass.h>
#include <Engine/Render/RenderTypes.h>	// RenderKey

#include <vector>

namespace engine
{
	class Renderer;
	class RenderTarget;
	class RenderTargetGroup;

	// Renderer를 서브메시 단위로 그리는 패스의 공통 구현.
	// - 항목을 캐싱하지 않는다. RenderManager가 매 프레임(View마다) Clear() 후
	//   컬링 통과분을 전개하며 Push()로 채운다.
	// - 흐름: Sort → BindTargets → DrawQueue
	class MeshRenderPass : public RenderPass
	{
		ENTITY_INFO(MeshRenderPass, RenderPass);
	public:

		// 정렬 계약 (깊이 계열 정책: FrontToBack / BackToFront / Custom)
		// - 같은 Renderer에서 나온 항목(이 패스에 참여하는 서브메시들)은 정렬 후 항상 연속하며,
		//   서브메시 인덱스 오름차순으로 그려진다.
		// - 따라서 한 Renderer의 서브메시를 '같은 위치에서 연달아 실행되는 단계'로 쓸 수 있다
		//   (예: SC Distortion의 Read(0) → Sync(1)).
		// - State 정책은 RenderKey(Material 상위) 순이므로 이 계약이 성립하지 않는다.
		enum class SortPolicy : uint8_t
		{
			State,			// sort_key = RenderKey. 배칭 최대화 (불투명, 알파 테스트)
			FrontToBack,	// 깊이 오름차순 (early-Z)
			BackToFront,	// 깊이 내림차순 (반투명, 페인터 순서)
			Custom,
		};

		MeshRenderPass(render_pass::ID id, SortPolicy sort_policy);
		virtual ~MeshRenderPass() override;

		void Clear() { queue_.clear(); }
		void Reserve(size_t count) { queue_.reserve(count); }

		// depth: RenderView::ComputeViewDepth 결과. State 정책에서는 사용하지 않는다.
		void Push(RenderKey key, uint32_t renderer_id, float depth, uint16_t obj_data_stride, uint32_t custom_sort_key, DrawFlags flags);

		virtual void Execute(const RenderContext& ctx) final;

		SortPolicy GetSortPolicy() const { return sort_policy_; }

		void SetOutputTarget(std::shared_ptr<RenderTargetGroup> rtg, const ClearSettings& clear_settings)
		{
			rtg_override_ = rtg;
			rtg_override_clear_setting_ = clear_settings;
		}

	protected:
		MeshRenderPass(const HashedStringView& concrete_class_name, render_pass::ID id, SortPolicy sort_policy);

		// 패스당 1회. 기본은 ctx.view의 출력 대상 바인딩.
		// 전용 대상(그림자 DSV, ROV UAV 등)을 쓰는 패스만 오버라이드하며, 그 대상의 클리어도 여기서 처리한다.
		virtual void BindTargets(const RenderContext& ctx);

		// 전용 대상을 바인딩했다면 UnbindTargets에서 해제할 것(특히 다음 패스가 SRV로 읽는 UAV)
		virtual void UnbindTargets(const RenderContext& ctx) {}	

	private:
		void Sort();
		void BuildGroups(const RenderContext& ctx);	// 사전 Buffer 용량 계산
		void UploadInstances(const RenderContext& ctx); // 사전 계산된 Buffer에 데이터 채워넣기 & Draw Command 생성
		void DrawGroups(const RenderContext& ctx);

	private:
		struct QueueItem
		{
			uint64_t sort_key;
			RenderKey key;
			uint32_t renderer_id;
			uint16_t obj_data_stride;
			DrawFlags flags; // uint16_t
		};
		static_assert(sizeof(QueueItem) == 24, "Keep size as small as possible");

		struct DrawGroup
		{
			uint32_t queue_begin = 0;							// queue_ 내 시작 인덱스
			uint32_t instance_count = 0;						// 그룹 인스턴스 수 (stride 0이어도 유효)
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