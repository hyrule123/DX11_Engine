#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Type.h>

#include <vector>

namespace engine
{
    class VertexShader;

    class InputLayout final 
        : public Resource
    {
        CLASS_INFO(InputLayout, Resource)
    public:
        InputLayout();
        virtual ~InputLayout() final;

        bool Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, s_ptr<VertexShader> vs);
        void Bind(const ComPtr<ID3D11DeviceContext>& context);

    private:
        ComPtr<ID3D11InputLayout> input_layout_ = {};
        w_ptr<VertexShader> vs_ = {};
    };
}


