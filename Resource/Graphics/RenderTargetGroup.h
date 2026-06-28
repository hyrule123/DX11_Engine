#pragma once

#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/DX11.h>

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

        void BindOutputMerger(ID3D11DeviceContext* context);

        void ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color);
        void ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8 stencil);

    private:
        RenderTargetArray render_target_buffers_ = {};
        s_ptr<DepthStencilView> dsv_ = {};

        std::array<ID3D11RenderTargetView*, kMaxRenderTargetCount> RTVs_ptr_cache_ = {};
    };
}


