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
        ENTITY_INFO(PixelShader, Shader)
    public:
        PixelShader();
        virtual ~PixelShader() final;

		bool IsReady() const { return (bool)ps_; }

        virtual void Bind(ID3D11DeviceContext* context) final;

    protected:
        virtual bool Create( const std::vector<uint8>& bytecode) final;

    private:
        ComPtr<ID3D11PixelShader> ps_ = {};
    };
}


