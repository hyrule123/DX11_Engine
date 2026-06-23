#pragma once

#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>

#include <array>

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11DeviceContext;

namespace engine
{
    class RenderTargetGroup 
        : public Resource
    {
        CLASS_INFO(RenderTargetGroup, Resource)
    public:
        using RTVArray = std::array<ComPtr<ID3D11RenderTargetView>, kMaxRenderTargetCount>;

        RenderTargetGroup();
        virtual ~RenderTargetGroup() override;

        void SetRenderTargets(RTVArray RTVs);
        void SetDepthStencilView(ComPtr<ID3D11DepthStencilView> DSV) { DSV_ = DSV; }

        void BindOutputMerger(ID3D11DeviceContext* context);

        void ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color);
        void ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8 stencil);

    private:
        RTVArray RTVs_ = {};
        ComPtr<ID3D11DepthStencilView> DSV_ = {};

        std::array<ID3D11RenderTargetView*, kMaxRenderTargetCount> RTVs_ptr_cache_ = {};
    };
}


