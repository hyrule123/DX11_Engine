#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>

#include <array>

struct ID3D11Texture2D;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

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

        void Bind(
            ID3D11DeviceContext* context, 
            UINT slot, 
            ShaderStageFlag stageflag = ShaderStage::kPS
        );

        static void BindTextures(
            ID3D11DeviceContext* context,
            const std::array<ID3D11ShaderResourceView*, kMaxTextureCount>& texture_srvs,
            ShaderStageFlag stageflag = ShaderStage::kPS
        );

        ID3D11ShaderResourceView* GetSRV() const { return SRV_.Get(); }

    private:
        ComPtr<ID3D11Texture2D>					tex2D_res_ = {};
        ComPtr<ID3D11ShaderResourceView>		SRV_ = {};

        UINT width_ = {};
        UINT height_ = {};
    };
}


