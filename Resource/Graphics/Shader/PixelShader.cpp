#include "Engine/Core/pch.h"
#include "PixelShader.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	PixelShader::PixelShader()
		: Shader(PixelShader::kClassConcreteName)
	{
	}

	PixelShader::~PixelShader()
	{}
	void PixelShader::Bind(ID3D11DeviceContext* context)
	{
		context->PSSetShader(ps_.Get(), nullptr, 0);
	}
	bool PixelShader::Create( const std::vector<uint8>& bytecode)
	{
		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreatePixelShader(bytecode.data(), bytecode.size(), nullptr, ps_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
		return true;
	}
}

