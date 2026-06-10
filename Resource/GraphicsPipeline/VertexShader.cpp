#include "Engine/Core/pch.h"
#include "VertexShader.h"

namespace engine
{
	VertexShader::VertexShader()
		: Shader("VertexShader")
	{

	}
	VertexShader::~VertexShader()
	{
	}
	void VertexShader::Bind(const ComPtr<ID3D11DeviceContext>& context)
	{
		context->VSSetShader(vs_.Get(), nullptr, 0);
	}
	bool VertexShader::Create(const ComPtr<ID3D11Device>& device, const std::vector<uint8>& bytecode)
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