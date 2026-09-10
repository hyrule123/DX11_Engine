#pragma once

#include <Engine/Core/Entity.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

#include <Engine/Collision/Collision.h>	//AABB2D

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine
{
	class RenderTargetGroup;
	class Renderer;

	struct RendererInfo
	{
		Renderer* renderer;
		AABB2D world_bounds;
		bool is_dirty;
	};

	class RenderPass
	{
	public:
		RenderPass(RenderPassOrder pass_order);
		virtual ~RenderPass();

		virtual void Init(ID3D11DeviceContext* context) {};

		virtual void Execute(ID3D11DeviceContext* context) = 0;

		RenderPassOrder GetPassOrder() const { return pass_order_; }

		void SetRenderTargetGroup(s_ptr<RenderTargetGroup> rt_group) { 
			render_target_group_ = std::move(rt_group); 
		}
		void BindRenderTargetGroup(ID3D11DeviceContext* context);

		uint32 AddRenderer(Renderer* renderer);
		void RemoveRenderer(uint32 renderer_slot);

		void MarkDirty(uint32 renderer_slot) {
			ASSERT(renderer_slot < registered_renderers_.size());
			registered_renderers_[renderer_slot].is_dirty = true;
		}

	private:
		RenderTargetGroup* GetRenderTargetGroup() const { return render_target_group_.get(); }

		RenderPassOrder pass_order_;
		s_ptr<RenderTargetGroup> render_target_group_ = {};

		std::vector<RendererInfo> registered_renderers_ = {};
	};
}