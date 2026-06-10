#include "Engine/Core/pch.h"
#include "PixelShader.h"

namespace engine
{
	PixelShader::PixelShader()
		: Shader("PixelShader")
	{
	}

	PixelShader::~PixelShader()
	{}
	void PixelShader::Bind(const ComPtr<ID3D11DeviceContext>& context)
	{
		context->PSSetShader(ps_.Get(), nullptr, 0);
	}
	bool PixelShader::Create(const ComPtr<ID3D11Device>& device, const std::vector<uint8>& bytecode)
	{
		HRESULT hr = device->CreatePixelShader(bytecode.data(), bytecode.size(), nullptr, ps_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
		return true;
	}
}

