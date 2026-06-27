#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/CoreMinimal.h>

#include <vector>

struct ID3D11Device;

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

        bool Create(ID3D11Device* device, const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, s_ptr<VertexShader> vs);
        bool Create(ID3D11Device* device, const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, const stdfs::path& vs_path);
        void Bind(ID3D11DeviceContext* context);

    private:
        ComPtr<ID3D11InputLayout> input_layout_ = {};
        w_ptr<VertexShader> vs_ = {};
    };
}


