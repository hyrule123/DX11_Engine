#include "Engine/Core/pch.h"
#include "RenderPass.h"

#include <Engine/Resource/GPU/RenderTargetGroup.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

#include <Engine/Game/Component/Renderer.h>



namespace engine
{
	RenderPass::RenderPass(RenderPassOrder pass_order)
		: pass_order_(pass_order)
	{}

	RenderPass::~RenderPass()
	{}
	void RenderPass::BindRenderTargetGroup(ID3D11DeviceContext* context)
	{
		if (render_target_group_)
		{
			render_target_group_->BindOutputMerger(context);
		}
	}
	void RenderPass::AddRenderer(Renderer* renderer)
	{
		ASSERT(renderer);
		registered_renderers_.push_back({ .renderer = renderer, .world_bounds = {}, .is_dirty = true });
		const size_t slot = registered_renderers_.size() - 1;
		renderer->SetRenderSlot(pass_order_, (uint32)slot);
	}
	void RenderPass::RemoveRenderer(Renderer* renderer)
	{
		ASSERT(renderer);
		const uint32 renderer_slot = renderer->GetRenderSlot(pass_order_);

		ASSERT(renderer_slot < registered_renderers_.size());
		ASSERT(registered_renderers_[renderer_slot].renderer == renderer);

		// swap and pop
		registered_renderers_[renderer_slot] = registered_renderers_.back();

		// 바뀐 slot 번호로 갱신
		registered_renderers_[renderer_slot].renderer->SetRenderSlot(pass_order_, renderer_slot);
		registered_renderers_.pop_back();

		// 제거 대상의 RenderSlot을 무효화
		renderer->InvalidateRenderSlot(pass_order_);
	}
}

