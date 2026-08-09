#include "Engine/Core/pch.h"
#include "Texture2D.h"

#include <DirectXTex/Include/DirectXTex.h>

#if defined(_WIN64) && defined(_DEBUG)
#pragma comment (lib, "DirectXTex/Lib/x64/Debug/DirectXTex.lib")
#endif

#if defined(_WIN64) && defined(_NDEBUG)
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
	bool Texture2D::LoadFromFile(const stdfs::path& res_path)
	{
		if (false == Super::LoadFromFile(res_path)) { return false; }

		if (false == res_path.has_extension())
		{
			ERROR_MESSAGE("확장자가 없어 로딩 실패");
			return false; 
		}

		s_ptr<DirectX::ScratchImage> img = LoadScratchImageFromFile(res_path);

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
			shader_resource_view_.ReleaseAndGetAddressOf()
		);

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		ComPtr<ID3D11Resource> temp = {};
		shader_resource_view_->GetResource(temp.GetAddressOf());
		temp.As(&tex2D_buffer_);
		if (nullptr == tex2D_buffer_)
		{
			return false;
		}

		return true;
	}
	void Texture2D::BindSRV(ID3D11DeviceContext* context, UINT slot, ShaderStage::Flags stageflag)
	{
		if (stageflag & ShaderStage::kVS)
		{
			context->VSSetShaderResources(slot, 1, shader_resource_view_.GetAddressOf());
		}
		if (stageflag & ShaderStage::kGS)
		{
			context->GSSetShaderResources(slot, 1, shader_resource_view_.GetAddressOf());
		}
		if (stageflag & ShaderStage::kPS)
		{
			context->PSSetShaderResources(slot, 1, shader_resource_view_.GetAddressOf());
		}
		if (stageflag & ShaderStage::kCS)
		{
			context->CSSetShaderResources(slot, 1, shader_resource_view_.GetAddressOf());
		}
	}

	void Texture2D::BindSRVs(ID3D11DeviceContext* context, const std::array<ID3D11ShaderResourceView*, kMaxTextureCount>& texture_srvs, ShaderStage::Flags stageflag)
	{
		UINT texcount = (UINT)texture_srvs.size();
		if (stageflag & ShaderStage::kVS)
		{
			context->VSSetShaderResources(0u, texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kGS)
		{
			context->GSSetShaderResources(0u, texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kPS)
		{
			context->PSSetShaderResources(0u, texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kCS)
		{
			context->CSSetShaderResources(0u, texcount, texture_srvs.data());
		}
	}

	bool Texture2D::CreateTexture2D(
		
		D3D11_TEXTURE2D_DESC* desc,
		const D3D11_SUBRESOURCE_DATA* initial_data
	)
	{
		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateTexture2D(desc, initial_data, tex2D_buffer_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		width_ = desc->Width;
		height_ = desc->Height;

		return true;
	}
	bool Texture2D::CreateSRV(D3D11_SHADER_RESOURCE_VIEW_DESC* srv_desc)
	{
		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateShaderResourceView(tex2D_buffer_.Get(), srv_desc, shader_resource_view_.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
		return true;
	}
	bool Texture2D::Resize(uint32 width, uint32 height)
	{
		if (!tex2D_buffer_)
		{
			DEBUG_MESSAGE("Texture2D가 존재하지 않습니다. Resize를 수행할 수 없습니다.");
			return false;
		}

		D3D11_TEXTURE2D_DESC desc = {};
		tex2D_buffer_->GetDesc(&desc);

		desc.Width = width;
		desc.Height = height;

		bool result = CreateTexture2D(&desc);
		if (false == result)
		{
			ASSERT(false);
			return false;
		}

		if (shader_resource_view_)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
			shader_resource_view_->GetDesc(&srv_desc);

			result = CreateSRV(&srv_desc);
			if (false == result)
			{
				ASSERT(false);
				return false;
			}
		}

		return true;
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
	s_ptr<DirectX::ScratchImage> Texture2D::LoadScratchImageFromFile(const stdfs::path& res_path)
	{
		using namespace DirectX;
		s_ptr<DirectX::ScratchImage> img = std::make_shared<ScratchImage>();

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
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return img;
	}
}

