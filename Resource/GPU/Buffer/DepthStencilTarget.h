#pragma once
#include <Engine/Resource/GPU/Buffer/Texture2D.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

namespace engine
{
    class DepthStencilTarget :
        public Texture2D
    {
        ENTITY_INFO(DepthStencilTarget, Texture2D)
    public:
        DepthStencilTarget();
        virtual ~DepthStencilTarget() override;

        bool CreateDSV(D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc);

        ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return dsv_; }
        ID3D11DepthStencilView* GetRawDepthStencilView() const { return dsv_.Get(); }

		virtual bool Resize(uint32_t width, uint32_t height) override;

    private:
        ComPtr<ID3D11DepthStencilView> dsv_ = {};
    };
}



