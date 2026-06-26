#pragma once
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct IDXGISwapChain;

namespace engine
{
    class RenderTargetView :
        public Texture2D
    {
        CLASS_INFO(RenderTargetView, Texture2D)
    public:
        RenderTargetView();
        virtual ~RenderTargetView() override;

        //기본적인(가장 많이 사용하는) 렌더타겟을 생성
        bool CreateDefault(
            ID3D11Device* device, 
            uint32 width, 
            uint32 height,
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM
        );

        //모든 생성 설정을 일임
        bool Create(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& tex_desc);

        bool CreateForSwapchain(ID3D11Device* device, ComPtr<IDXGISwapChain> swap_chain);

        ComPtr<ID3D11RenderTargetView> GetRTV() const { return render_target_view_; }
        ID3D11RenderTargetView* GetRawRTV() const { return render_target_view_.Get(); }

    private:
        ComPtr<ID3D11RenderTargetView> render_target_view_ = {};
    };
}


