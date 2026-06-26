#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilState;

namespace engine
{
    class DepthStencilState :
        public Resource
    {
        CLASS_INFO(DepthStencilState, Resource)
    public:
        DepthStencilState();
        virtual ~DepthStencilState() override;

        bool Create(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC& desc);

        void SetDepthStencilState(ComPtr<ID3D11DepthStencilState> dss) {
            ds_state_ = std::move(dss);
        }

        void Bind(ID3D11DeviceContext* context, uint32 stencil_ref = 1u);

    private:
        ComPtr<ID3D11DepthStencilState> ds_state_ = {};
    };
}


