#pragma once
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

namespace engine
{
    class DepthStencilView :
        public Texture2D
    {
        CLASS_INFO(DepthStencilView, Texture2D)
    public:
        DepthStencilView();
        virtual ~DepthStencilView() override;

        //DSV Format은 Texture2D Format 보고 알아서 결정
        //TYPELESS Format을 사용할 것 (DXGI_FORMAT_R24G8_TYPELESS)
        bool Create(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc);

        ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return dsv_; }
        ID3D11DepthStencilView* GetRawDepthStencilView() const { return dsv_.Get(); }

    private:
        ComPtr<ID3D11DepthStencilView> dsv_ = {};
    };
}



