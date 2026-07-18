#pragma once

#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/DX11.h>
#include <Engine/Core/Enum.h>

#include <array>

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DeviceContext;

namespace engine
{
    class RenderTargetView;
    class DepthStencilView;

    class RenderTargetGroup 
        : public Resource
    {
        CLASS_INFO(RenderTargetGroup, Resource)
    public:
        using RenderTargetArray = std::array<s_ptr<RenderTargetView>, kMaxRenderTargetCount>;

        RenderTargetGroup();
        virtual ~RenderTargetGroup() override;

        void SetRenderTargets(const RenderTargetArray& rtv_arr);
        void SetDepthStencilView(s_ptr<DepthStencilView> dsv) { 
            dsv_ = std::move(dsv);
        }

		void BindShaderResourceViews(ID3D11DeviceContext* context, ShaderStage::Flags stage_flags);
		void UnBindShaderResourceViews(ID3D11DeviceContext* context);

        void BindOutputMerger(ID3D11DeviceContext* context);
		void UnBindOutputMerger(ID3D11DeviceContext* context);


        void ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color);
        void ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8 stencil);

        void Reset();

		void SetRequiresResize(bool requires_resize) { requires_resize_ = requires_resize; }
		void Resize(ID3D11Device* device, uint32 width, uint32 height);

        //기본 Texture Size로 설정됨. override용
		const D3D11_VIEWPORT& GetViewport() const { return viewport_; }
		void SetViewport(const D3D11_VIEWPORT& viewport) { viewport_ = viewport; }

    private:
        RenderTargetArray render_target_buffers_ = {};
        s_ptr<DepthStencilView> dsv_ = {};

		std::array<ID3D11ShaderResourceView*, kMaxRenderTargetCount> SRVs_ptr_cache_ = {};
        std::array<ID3D11RenderTargetView*, kMaxRenderTargetCount> RTVs_ptr_cache_ = {};

        D3D11_VIEWPORT viewport_ = {};

        bool requires_resize_ = {};

		ShaderStage::Flags last_bound_stage_flags_ = {};
    };
}


