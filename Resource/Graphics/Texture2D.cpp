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

#include <d3d11.h>

namespace engine
{
	Texture2D::Texture2D()
		: Super(STRINGIFY(Texture2D))
	{}

	Texture2D::Texture2D(const std::string_view concrete_class_name)
		: Super(concrete_class_name)
	{
	}

	Texture2D::~Texture2D()
	{}
	bool Texture2D::LoadFromFile(const stdfs::path & path)
	{
		if (false == Super::LoadFromFile(path)) { return false; }

		if (false == path.has_extension()) 
		{
			ERROR_MESSAGE("확장자가 없어 로딩 실패");
			return false; 
		}

		DirectX::ScratchImage img = {};
		std::wstring upper_ext = string_utils::GetUpperCase(path.extension().wstring());
		HRESULT hr = E_FAIL;

		if (upper_ext == L".DDS")
		{
			hr = LoadFromDDSFile(path.wstring().c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, nullptr, img);
		}
		else if (upper_ext == L".TGA")
		{
			hr = LoadFromTGAFile(path.wstring().c_str(), DirectX::TGA_FLAGS::TGA_FLAGS_NONE, nullptr, img);
		}
		else // WIC (png, jpg, jpeg, bmp )
		{
			hr = LoadFromWICFile(path.wstring().c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, nullptr, img);
		}

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		// 메타데이터 저장 (클래스 멤버 변수로 선언되어 있다고 가정)
		const DirectX::TexMetadata& meta = img.GetMetadata();
		width_ = static_cast<UINT>(meta.width);
		height_ = static_cast<UINT>(meta.height);

		//  리소스 및 SRV(Shader Resource View) 생성
		hr = DirectX::CreateShaderResourceView(
			GraphicsDevice::GetInst().GetDevice().Get(),
			img.GetImages(),
			img.GetImageCount(),
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
		temp.As(&tex2D_res_);
		if (nullptr == tex2D_res_)
		{
			return false;
		}

		return true;
	}
	void Texture2D::Bind(ID3D11DeviceContext* context, UINT slot, ShaderStageFlag stageflag)
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
	void Texture2D::BindTextures(ID3D11DeviceContext* context, 
		const std::array<ID3D11ShaderResourceView*, 
		kMaxTextureCount>& texture_srvs,
		ShaderStageFlag stageflag)
	{
		UINT texcount = (UINT)texture_srvs.size();
		if (stageflag & ShaderStage::kVS)
		{
			context->VSSetShaderResources(0u, (UINT)texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kGS)
		{
			context->GSSetShaderResources(0u, (UINT)texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kPS)
		{
			context->PSSetShaderResources(0u, (UINT)texcount, texture_srvs.data());
		}
		if (stageflag & ShaderStage::kCS)
		{
			context->CSSetShaderResources(0u, (UINT)texcount, texture_srvs.data());
		}
	}
	ComPtr<ID3D11Texture2D> Texture2D::CreateTexture2D(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc)
	{
		HRESULT hr = device->CreateTexture2D(&desc, nullptr, tex2D_res_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return tex2D_res_;
	}
}

