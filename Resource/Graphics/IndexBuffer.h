#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine
{
    class IndexBuffer :
        public Resource
    {
        CLASS_INFO(IndexBuffer, Resource)
    public:
        IndexBuffer();
        virtual ~IndexBuffer() override;

        bool Create(ID3D11Device* device, const std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY topology);

        void Bind(ID3D11DeviceContext* context);

        UINT GetIndexCount() const { return (UINT)indices_.size(); }

    private:
        std::vector<UINT> indices_ = {};
        D3D11_PRIMITIVE_TOPOLOGY topology_ = {};
        ComPtr<ID3D11Buffer> index_buffer_ = {};
    };
}

