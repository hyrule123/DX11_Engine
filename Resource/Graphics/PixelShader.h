#pragma once
#include "Shader.h"

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/UtilMacro.h>

struct ID3D11Device;

namespace engine
{
    class PixelShader final 
        : public Shader
    {
        CLASS_INFO(PixelShader, Shader)
    public:
        PixelShader();
        virtual ~PixelShader() final;

        virtual void Bind(ID3D11DeviceContext* context) final;

    protected:
        virtual bool Create(ID3D11Device* device, const std::vector<uint8>& bytecode) final;

    private:
        ComPtr<ID3D11PixelShader> ps_ = {};
    };
}


