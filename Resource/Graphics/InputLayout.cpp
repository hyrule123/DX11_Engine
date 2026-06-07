#include "Engine/pch.h"
#include "InputLayout.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Resource/Graphics/VertexShader.h>

namespace engine
{
	InputLayout::InputLayout()
		: Resource("InputLayout")
	{
	}
	InputLayout::~InputLayout()
	{
	}
	bool InputLayout::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, s_ptr<VertexShader> vs)
	{
		ASSERT_MESSAGE(!desc.empty() && vs, "desc와 Vertex Shader 둘 다 필요");

		ComPtr<ID3D11Device> device = GraphicsDevice::GetInst().GetDevice();

		const auto& bytecode = vs->GetByteCode();

		HRESULT hr = device->CreateInputLayout(desc.data(), (UINT)desc.size(), bytecode.data(), bytecode.size(), input_layout_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		vs_ = vs;

		return true;
	}
	void InputLayout::Bind(const ComPtr<ID3D11DeviceContext>& context)
	{
		context->IASetInputLayout(input_layout_.Get());
	}
}