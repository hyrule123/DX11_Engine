#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>

#include <d3d11.h>

namespace engine
{
    class Texture2D :
        public Resource
    {
        CLASS_INFO(TextureBase, Resource)
    public:
        Texture2D();
        virtual ~Texture2D() override;

        virtual bool LoadFromFile(const stdfs::path& path) override;

    private:
        ComPtr<ID3D11Texture2D>					tex2D_res_ = {};
        ComPtr<ID3D11ShaderResourceView>		SRV_ = {};

        UINT width_ = {};
        UINT height_ = {};
    };
}


