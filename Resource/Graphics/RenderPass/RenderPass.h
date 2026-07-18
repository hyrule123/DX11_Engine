#pragma once

#include <Engine/Core/Entity.h>
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine
{
	class RenderTargetGroup;
	class RenderPass
		: public Entity
	{
		CLASS_INFO(RenderPass, Entity)
	public:
		RenderPass(const std::string_view concrete_class_name, RenderPassOrder pass_order);
		virtual ~RenderPass() override;

		virtual void Init(ID3D11Device* device, ID3D11DeviceContext* context) {};

		virtual void Execute(ID3D11Device* device, ID3D11DeviceContext* context) = 0;

		RenderPassOrder GetPassOrder() const { return pass_order_; }

		void SetRenderTargetGroup(s_ptr<RenderTargetGroup> rt_group) { 
			render_target_group_ = std::move(rt_group); 
		}
		void BindRenderTargetGroup(ID3D11DeviceContext* context);
	private:
		RenderTargetGroup* GetRenderTargetGroup() const { return render_target_group_.get(); }

		RenderPassOrder pass_order_;
		s_ptr<RenderTargetGroup> render_target_group_ = {};
	};
}