#pragma once
#include "Shader.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <Engine/Header/Type.h>

namespace engine
{
    class PixelShader final 
        : public Shader
    {
    public:
        PixelShader();
        virtual ~PixelShader() final;

        virtual void Bind(const ComPtr<ID3D11DeviceContext>& context) final;

    protected:
        virtual bool Create(const ComPtr<ID3D11Device>& device, const std::vector<uint8>& bytecode) final;

    private:
        ComPtr<ID3D11PixelShader> ps_ = {};
    };
}


