#include "Engine/Core/pch.h"
#include "VertexShader.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	VertexShader::VertexShader()
		: Shader(STRINGIFY(VertexShader))
	{

	}
	VertexShader::~VertexShader()
	{
	}
	void VertexShader::Bind(ID3D11DeviceContext* context)
	{
		context->VSSetShader(vs_.Get(), nullptr, 0);
	}
	bool VertexShader::Create( const std::vector<uint8>& bytecode)
	{
		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateVertexShader(bytecode.data(), bytecode.size(), nullptr, vs_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		byte_code_ = bytecode;

		return true;
	}
}