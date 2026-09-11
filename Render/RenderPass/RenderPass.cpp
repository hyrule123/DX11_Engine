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
	uint32 RenderPass::AddRenderer(Renderer* renderer)
	{
		ASSERT(renderer);
		registered_renderers_.push_back({ .renderer = renderer, .world_bounds = {}, .is_dirty = true });
		return (uint32)(registered_renderers_.size() - 1);
	}
	void RenderPass::RemoveRenderer(uint32 renderer_slot)
	{
		ASSERT(renderer_slot < registered_renderers_.size());

		// swap and pop
		registered_renderers_[renderer_slot] = registered_renderers_.back();
		registered_renderers_[renderer_slot].renderer->SetRenderSlot(pass_order_, renderer_slot);
		registered_renderers_.pop_back();
	}
}

