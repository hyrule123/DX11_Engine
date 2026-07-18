#pragma once
#include "RenderPass.h"

namespace engine
{
	class Mesh;
    class GraphicsShaderSet;
	class RenderTargetGroup;

    class PresentPass final : 
        public RenderPass
    {
		CLASS_INFO(PresentPass, RenderPass)
    public:
        PresentPass();
		virtual ~PresentPass() override;

		virtual void Init(ID3D11Device* device, ID3D11DeviceContext* context) override;
		virtual void Execute(ID3D11Device* device, ID3D11DeviceContext* context) override;

		// src_render_target_이 없으면 작동하지 않음
		bool IsSet() const { return src_render_target_ != nullptr; }
		
		//복사 원본
		void SetSourceRenderTarget(s_ptr<RenderTargetGroup> rt_group) {
			src_render_target_ = std::move(rt_group);
		}

    private:
		u_ptr<Mesh> mesh_ = {};
		u_ptr<GraphicsShaderSet> shader_set_ = {};
		s_ptr<RenderTargetGroup> src_render_target_ = {};
    };
}


