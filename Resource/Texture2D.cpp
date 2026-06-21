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
		: Super(STRINGIFY(TextureBase))
	{}

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

		// 3. 메타데이터 저장 (클래스 멤버 변수로 선언되어 있다고 가정)
		const DirectX::TexMetadata& meta = img.GetMetadata();
		width_ = static_cast<UINT>(meta.width);
		height_ = static_cast<UINT>(meta.height);

		//  리소스 및 SRV(Shader Resource View) 생성
		// m_SRV는 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> 타입으로 가정합니다.
		hr = DirectX::CreateShaderResourceView(
			GraphicsDevice::GetInst().GetDevice().Get(),
			img.GetImages(),
			img.GetImageCount(),
			meta,
			SRV_.ReleaseAndGetAddressOf()
		);

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		ComPtr<ID3D11Resource> temp = {};
		SRV_->GetResource(temp.GetAddressOf());
		temp.As(&tex2D_res_);
		if (nullptr == tex2D_res_)
		{
			return false;
		}

		return true;
	}
}

