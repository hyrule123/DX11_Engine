#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11BlendState;

namespace engine
{

    class BlendState :
        public Resource
    {
        CLASS_INFO(BlendState, Resource)
    public:
        BlendState();
        virtual ~BlendState() override;

		bool IsReady() const { return (bool)blend_state_; }

        bool Create(ID3D11Device* device, const D3D11_BLEND_DESC& desc);
        void SetBlenState(ComPtr<ID3D11BlendState> bss) {
            blend_state_ = bss;
        }

        void Bind(
            ID3D11DeviceContext* context, 
            const float* blend_factor = kDefaultBlendFactor, 
            uint32 sample_mask = kDefaultSampleMask
        );

    private:
        ComPtr<ID3D11BlendState> blend_state_ = {};
    };
}


