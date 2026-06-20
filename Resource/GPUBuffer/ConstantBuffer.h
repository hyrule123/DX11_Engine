#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Resource/Resource.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace engine
{
    class ConstantBuffer :
        public Resource
    {
        CLASS_INFO(ConstantBuffer, Resource)

    public:
        ConstantBuffer();
        virtual ~ConstantBuffer() override;
        
        bool Create(size_t stride);
        template <typename T>
        bool Create() {
            return Create(sizeof(T));
        }

        void Upload(ID3D11DeviceContext* context, const void* ptr, size_t size);
        template <typename T>
        void Upload(ID3D11DeviceContext* context, const T& data) {
            Upload(context, &data, sizeof(T));
        }

        void Bind(ID3D11DeviceContext* context, ShaderStageFlag flag, UINT slot);

    private:
        ComPtr<ID3D11Buffer> buffer_ = {};
        size_t stride_ = {};
    };
}


