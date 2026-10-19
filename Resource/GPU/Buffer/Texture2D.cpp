#include "Engine/Core/pch.h"
#include "Texture2D.h"

#include <DirectXTex/Include/DirectXTex.h>

#if defined(_WIN64) && defined(_DEBUG)
#pragma comment (lib, "DirectXTex/Lib/x64/Debug/DirectXTex.lib")
#endif

#if defined(_WIN64) && defined(NDEBUG)
#pragma comment (lib, "DirectXTex/Lib/x64/Release/DirectXTex.lib")
#endif

#ifndef _WIN64
	#error("x64만 지원")
#endif

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Util/string_utils.h>

#include <Engine/Core/Debug.h>


namespace engine
{
	Texture2D::Texture2D()
		: Super(Texture2D::kClassConcreteName)
	{}

	Texture2D::Texture2D(const HashedStringView& concrete_class_name)
		: Super(concrete_class_name)
	{
	}

	Texture2D::~Texture2D()
	{}
	bool Texture2D::LoadFromFile(const std::filesystem::path& res_path)
	{
		if (false == Super::LoadFromFile(res_path)) { return false; }

		if (false == res_path.has_extension())
		{
			ERR_MSG("확장자가 없어 로딩 실패");
			return false; 
		}

		std::shared_ptr<DirectX::ScratchImage> img = LoadScratchImageFromFile(res_path);

		// 메타데이터 저장 (클래스 멤버 변수로 선언되어 있다고 가정)
		const DirectX::TexMetadata& meta = img->GetMetadata();
		width_ = static_cast<UINT>(meta.width);
		height_ = static_cast<UINT>(meta.height);

		//  리소스 및 SRV(Shader Resource View) 생성
		HRESULT hr = DirectX::CreateShaderResourceView(
			GraphicsDevice::GetInst().GetDevice(),
			img->GetImages(),
			img->GetImageCount(),
			meta,
			SRV_.ReleaseAndGetAddressOf()
		);

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		ComPtr<ID3D11Resource> temp = {};
		SRV_->GetResource(temp.GetAddressOf());
		temp.As(&tex2D_buffer_);
		if (nullptr == tex2D_buffer_)
		{
			return false;
		}

		return true;
	}

	bool Texture2D::SaveToFile(const std::filesystem::path& res_path)
	{
		auto device = GraphicsDevice::GetInst().GetDevice();
		auto context = GraphicsDevice::GetInst().GetContext();

		DirectX::ScratchImage image;
		DirectX::CaptureTexture(device, context, tex2D_buffer_.Get(), image);
		HRESULT result = DirectX::SaveToWICFile(
			*image.GetImage(0, 0, 0), 
			DirectX::WIC_FLAGS_NONE,
			DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), 
			res_path.wstring().c_str()
		);

		if (FAILED(result))
		{
			ERR_MSG_HRESULT(result);
			return false;
		}

		return true;
	}

	void Texture2D::BindSRV(ID3D11DeviceContext* context, ShaderStageFlags stage_flags, UINT slot)
	{
		if (stage_flags.Test(ShaderStage::Vertex))
		{
			context->VSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
		if (stage_flags.Test(ShaderStage::Geometry))
		{
			context->GSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
		if (stage_flags.Test(ShaderStage::Pixel))
		{
			context->PSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
		if (stage_flags.Test(ShaderStage::Compute))
		{
			context->CSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
	}

	bool Texture2D::CreateTexture2D(
		D3D11_TEXTURE2D_DESC* desc,
		const D3D11_SUBRESOURCE_DATA* initial_data
	)
	{
		if (desc == nullptr)
		{
			ERR_MSG("CreateTexture2D: desc is nullptr");
			return false;
		}

		ComPtr<ID3D11Texture2D> tex = CreateTexture2DImpl(desc, initial_data);
		ComPtr<ID3D11ShaderResourceView> srv = nullptr;
		ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
		if (tex)
		{
			// 기본 SRV 생성
			if(desc->BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				srv = CreateSRVImpl(tex.Get(), nullptr);
				if (srv == nullptr)
				{
					ERR_MSG("CreateTexture2D: Failed to create default SRV");
					return false;
				}
			}
			// 기본 UAV 생성
			if (desc->BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
				uav_desc.Format = desc->Format;
				uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				uav_desc.Texture2D.MipSlice = 0;
				uav = CreateUAVImpl(tex.Get(), &uav_desc);

				if (uav == nullptr)
				{
					ERR_MSG("CreateTexture2D: Failed to create default UAV");
					return false;
				}
			}

			tex2D_buffer_ = std::move(tex);
			SRV_ = std::move(srv);
			UAV_ = std::move(uav);
			width_ = desc->Width;
			height_ = desc->Height;
		}
		return true;
	}
	ComPtr<ID3D11Texture2D> Texture2D::CreateTexture2DImpl(D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initial_data)
	{
		ComPtr<ID3D11Texture2D> tex;
		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateTexture2D(desc, initial_data, tex.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}
		return tex;
	}
	bool Texture2D::CreateSRV(D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc)
	{
		ComPtr<ID3D11ShaderResourceView> srv = CreateSRVImpl(tex2D_buffer_.Get(), srv_desc);
		if (srv)
		{
			SRV_ = std::move(srv);
			return true;
		}
		return false;
	}
	bool Texture2D::CreateUAV(D3D11_UNORDERED_ACCESS_VIEW_DESC* uav_desc)
	{
		ComPtr<ID3D11UnorderedAccessView> uav = CreateUAVImpl(tex2D_buffer_.Get(), uav_desc);
		if (uav)
		{
			UAV_ = std::move(uav);
			return true;
		}
		return false;
	}
	void Texture2D::BindUAV(ID3D11DeviceContext* context, RegisterU slot)
	{
		context->CSSetUnorderedAccessViews(slot.Get(), 1, UAV_.GetAddressOf(), nullptr);
	}
	void Texture2D::UnbindUAV(ID3D11DeviceContext* context, RegisterU slot)
	{
		ID3D11UnorderedAccessView* null_uav = nullptr;
		context->CSSetUnorderedAccessViews(slot.Get(), 1, &null_uav, nullptr);
	}
	ComPtr<ID3D11ShaderResourceView> Texture2D::CreateSRVImpl(ID3D11Texture2D* texture, D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc)
	{
		if (texture == nullptr)
		{
			ERR_MSG("CreateSRVImpl: texture is nullptr");
			return nullptr;
		}
		ComPtr<ID3D11ShaderResourceView> srv;
		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateShaderResourceView(texture, srv_desc, srv.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}
		return srv;
	}

	ComPtr<ID3D11UnorderedAccessView> Texture2D::CreateUAVImpl(ID3D11Texture2D* texture, D3D11_UNORDERED_ACCESS_VIEW_DESC* uav_desc)
	{
		if (texture == nullptr)
		{
			ERR_MSG("CreateUAVImpl: texture is nullptr");
			return nullptr;
		}
		ComPtr<ID3D11UnorderedAccessView> uav;
		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateUnorderedAccessView(texture, uav_desc, uav.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}
		return uav;
	}

	bool Texture2D::Resize(uint32_t width, uint32_t height)
	{
		if (!tex2D_buffer_)
		{
			ASSERT_F(false, "Texture2D does not exist. Cannot perform Resize.");
			return false;
		}

		D3D11_TEXTURE2D_DESC desc = {};
		tex2D_buffer_->GetDesc(&desc);

		desc.Width = width;
		desc.Height = height;

		ComPtr<ID3D11Texture2D> new_texture = CreateTexture2DImpl(&desc);
		if (new_texture == nullptr)
		{
			ERR_MSG("Texture2D Resize 실패: 새로운 텍스처 생성 실패");
			return false;
		}

		ComPtr<ID3D11ShaderResourceView> new_srv = nullptr;
		if (SRV_)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			SRV_->GetDesc(&srv_desc);

			new_srv = CreateSRVImpl(new_texture.Get(), &srv_desc);
			if (new_srv == nullptr)
			{
				ERR_MSG("Texture2D Resize 실패: 새로운 SRV 생성 실패");
				return false;
			}
		}
		ComPtr<ID3D11UnorderedAccessView> new_uav = nullptr;
		if(UAV_)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
			UAV_->GetDesc(&uav_desc);

			new_uav = CreateUAVImpl(new_texture.Get(), &uav_desc);
			if (new_uav == nullptr)
			{
				ERR_MSG("Texture2D Resize 실패: 새로운 UAV 생성 실패");
				return false;
			}
		}

		tex2D_buffer_ = std::move(new_texture);
		SRV_ = std::move(new_srv);
		UAV_ = std::move(new_uav);
		width_ = width;
		height_ = height;
		return true;
	}

	void Texture2D::CopyFrom(ID3D11DeviceContext* context, const Texture2D& src)
	{
		ASSERT(context);
		ASSERT_F(this != &src, "Texture2D::CopyFrom: 자기 자신으로는 복사할 수 없음");
		ASSERT_F(tex2D_buffer_ && src.tex2D_buffer_, "Texture2D::CopyFrom: 텍스처가 생성되지 않음");

#if defined(_DEBUG)
		// CopyResource 요구 조건: 리소스 타입·크기·포맷 호환, 밉/배열/샘플 일치
		D3D11_TEXTURE2D_DESC dst_desc = {};
		D3D11_TEXTURE2D_DESC src_desc = {};
		tex2D_buffer_->GetDesc(&dst_desc);
		src.tex2D_buffer_->GetDesc(&src_desc);

		ASSERT_F(dst_desc.Width == src_desc.Width && dst_desc.Height == src_desc.Height,
			"Texture2D::CopyFrom: 크기 불일치 ({}x{} <- {}x{})", dst_desc.Width, dst_desc.Height, src_desc.Width, src_desc.Height);
		ASSERT_F(dst_desc.Format == src_desc.Format,
			"Texture2D::CopyFrom: 포맷 불일치 ({} <- {})", (uint32_t)dst_desc.Format, (uint32_t)src_desc.Format);
		ASSERT_F(dst_desc.MipLevels == src_desc.MipLevels && dst_desc.ArraySize == src_desc.ArraySize,
			"Texture2D::CopyFrom: 밉 레벨 또는 배열 크기 불일치");
		ASSERT_F(dst_desc.SampleDesc.Count == src_desc.SampleDesc.Count && dst_desc.SampleDesc.Quality == src_desc.SampleDesc.Quality,
			"Texture2D::CopyFrom: 샘플 설정 불일치");
		ASSERT_F(dst_desc.Usage != D3D11_USAGE_IMMUTABLE, "Texture2D::CopyFrom: IMMUTABLE 텍스처에는 복사할 수 없음");
#endif

		context->CopyResource(tex2D_buffer_.Get(), src.tex2D_buffer_.Get());
	}

	void Texture2D::SetTexture2D(ComPtr<ID3D11Texture2D> texture)
	{
		if (texture)
		{
			D3D11_TEXTURE2D_DESC desc;
			texture->GetDesc(&desc);
			width_ = desc.Width;
			height_ = desc.Height;
		}
		else
		{
			width_ = 0u;
			height_ = 0u;
		}
	}
	std::shared_ptr<DirectX::ScratchImage> Texture2D::LoadScratchImageFromFile(const std::filesystem::path& res_path)
	{
		using namespace DirectX;
		std::shared_ptr<DirectX::ScratchImage> img = std::make_shared<ScratchImage>();

		std::wstring upper_ext = string_utils::GetUpperCase(res_path.extension().wstring());
		HRESULT hr = E_FAIL;

		if (upper_ext == L".DDS")
		{
			hr = LoadFromDDSFile(res_path.wstring().c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, *img);
		}
		else if (upper_ext == L".TGA")
		{
			hr = LoadFromTGAFile(res_path.wstring().c_str(), DirectX::TGA_FLAGS::TGA_FLAGS_NONE, nullptr, *img);
		}
		else // WIC (png, jpg, jpeg, bmp )
		{
			hr = LoadFromWICFile(res_path.wstring().c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, nullptr, *img);
		}

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}

		return img;
	}
}

