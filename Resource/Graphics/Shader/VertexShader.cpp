#include "Engine/Core/pch.h"
#include "VertexShader.h"

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
	bool VertexShader::Create(ID3D11Device* device, const std::vector<uint8>& bytecode)
	{
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