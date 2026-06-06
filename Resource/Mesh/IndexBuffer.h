#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Common.h>

#include <d3d11.h>

namespace engine
{
    class IndexBuffer :
        public Resource
    {
    public:
        IndexBuffer();
        virtual ~IndexBuffer() override;

        bool Create(const std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY topology);

    private:
        std::vector<UINT> indices_ = {};
        D3D11_PRIMITIVE_TOPOLOGY topology_ = {};
        ComPtr<ID3D11Buffer> index_buffer_ = {};
    };
}

