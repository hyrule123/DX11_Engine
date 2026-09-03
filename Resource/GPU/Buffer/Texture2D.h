#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Enum.h>
#include <Engine/Core/DX11.h>

#include <array>

namespace DirectX
{
    class ScratchImage;
}

namespace engine
{
    class Texture2D
        : public Resource
        , public ShaderResource
    {
        ENTITY_INFO(Texture2D, Resource)
    public:
        Texture2D();
        Texture2D(const HashedStringView& concrete_class_name);
        virtual ~Texture2D() override;

        virtual bool LoadFromFile(const stdfs::path& res_path) override;
		bool SaveToFile(const stdfs::path& res_path);

        void BindSRV(
            ID3D11DeviceContext* context, 
            UINT slot, 
            ShaderStage::Flags stageflag = ShaderStage::kPS
        );

        static void BindSRVs(
            ID3D11DeviceContext* context,
            const std::array<ID3D11ShaderResourceView*, kMaxTextureCount>& texture_srvs,
            ShaderStage::Flags stageflag = ShaderStage::kPS
        );

        ComPtr<ID3D11ShaderResourceView> GetSRV() const { return SRV_; }
        ID3D11ShaderResourceView* GetRawSRV() const { return SRV_.Get(); }

        // BindFlag에 따라 기본 SRV, UAV가 생성됨.
        bool CreateTexture2D(
            D3D11_TEXTURE2D_DESC* desc,
            const D3D11_SUBRESOURCE_DATA* initial_data = nullptr
        );

        // 기본 SRV/UAV는 생성되어 있음. 별도 설정으로 생성하길 원한다면 호출할 것
        bool CreateSRV(D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc);

        // 기본 SRV/UAV는 생성되어 있음. 별도 설정으로 생성하길 원한다면 호출할 것
		bool CreateUAV(D3D11_UNORDERED_ACCESS_VIEW_DESC* uav_desc);

		void BindUAV(ID3D11DeviceContext* context, UINT slot);
		void UnbindUAV(ID3D11DeviceContext* context, UINT slot);

        // 상속 시 Super::Resize() 호출할 것
		virtual bool Resize(uint32 width, uint32 height);

        uint32 GetWidth() const { return width_; }
        uint32 GetHeight() const { return height_; }

    protected:
        void SetTexture2D(ComPtr<ID3D11Texture2D> texture);
        ComPtr<ID3D11Texture2D> GetTexture2D() const { return tex2D_buffer_; }

        void SetUAV(ComPtr<ID3D11UnorderedAccessView> unordered_access_view) {
            UAV_ = std::move(unordered_access_view);
        }
        void SetSRV(ComPtr<ID3D11ShaderResourceView> shader_resource_view) {
            SRV_ = std::move(shader_resource_view);
        }

        void SetSize(uint32 width, uint32 height) {
            width_ = width;
            height_ = height;
        }

        s_ptr<DirectX::ScratchImage> LoadScratchImageFromFile(const stdfs::path& res_path);
    private:
		ComPtr<ID3D11Texture2D> CreateTexture2DImpl(
			D3D11_TEXTURE2D_DESC* desc,
			const D3D11_SUBRESOURCE_DATA* initial_data = nullptr
		);
		ComPtr<ID3D11ShaderResourceView> CreateSRVImpl(ID3D11Texture2D* texture, D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc);
		ComPtr<ID3D11UnorderedAccessView> CreateUAVImpl(ID3D11Texture2D* texture, D3D11_UNORDERED_ACCESS_VIEW_DESC* uav_desc);

        ComPtr<ID3D11UnorderedAccessView>		UAV_ = {};
        ComPtr<ID3D11Texture2D>					tex2D_buffer_ = {};

        UINT width_ = {};
        UINT height_ = {};
    };
}


