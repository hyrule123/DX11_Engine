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

        bool CreateDSV(ID3D11Device* device, D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc);

        ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return dsv_; }
        ID3D11DepthStencilView* GetRawDepthStencilView() const { return dsv_.Get(); }

		virtual bool Resize(ID3D11Device* device, uint32 width, uint32 height) override;

    private:
        ComPtr<ID3D11DepthStencilView> dsv_ = {};
    };
}



