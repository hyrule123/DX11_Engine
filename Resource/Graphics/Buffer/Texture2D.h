#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/DX11.h>

#include <array>

struct ID3D11Texture2D;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

namespace DirectX
{
    class ScratchImage;
}

namespace engine
{
    class Texture2D :
        public Resource
    {
        CLASS_INFO(Texture2D, Resource)
    public:
        Texture2D();
        Texture2D(const std::string_view concrete_class_name);
        virtual ~Texture2D() override;

        virtual bool LoadFromFile(const stdfs::path& res_path) override;

        void BindSRV(
            ID3D11DeviceContext* context, 
            UINT slot, 
            ShaderStageFlag stageflag = ShaderStage::kPS
        );

        static void BindSRVs(
            ID3D11DeviceContext* context,
            const std::array<ID3D11ShaderResourceView*, kMaxTextureCount>& texture_srvs,
            ShaderStageFlag stageflag = ShaderStage::kPS
        );

        ID3D11ShaderResourceView* GetSRV() const { return shader_resource_view_.Get(); }

    protected:
        ComPtr<ID3D11Texture2D> CreateTexture2D(
            ID3D11Device* device,
            const D3D11_TEXTURE2D_DESC& desc
        );
        void SetTexture2D(ComPtr<ID3D11Texture2D> texture);
        ComPtr<ID3D11Texture2D> GetTexture2D() const { return tex2D_res_; }

        void SetShaderResourceView(ComPtr<ID3D11ShaderResourceView> shader_resource_view) {
            shader_resource_view_ = std::move(shader_resource_view);
        }

        void SetSize(uint32 width, uint32 height) {
            width_ = width;
            height_ = height;
        }
    protected:
        s_ptr<DirectX::ScratchImage> LoadScratchImageFromFile(const stdfs::path& res_path);

    private:
        ComPtr<ID3D11Texture2D>					tex2D_res_ = {};
        ComPtr<ID3D11ShaderResourceView>		shader_resource_view_ = {};

        UINT width_ = {};
        UINT height_ = {};
    };
}


