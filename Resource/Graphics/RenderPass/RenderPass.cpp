#include "Engine/Core/pch.h"
#include "RenderPass.h"

#include <Engine/Core/DX11.h>

#include <Engine/Resource/Graphics/RenderTargetGroup.h>

#include <Engine/Core/Debug.h>

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
}

