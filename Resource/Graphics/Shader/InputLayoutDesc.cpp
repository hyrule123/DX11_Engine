#include "Engine/Core/pch.h"
#include "InputLayoutDesc.h"

#include <Engine/Resource/Graphics/Shader/VertexShader.h>

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	InputLayoutDesc::InputLayoutDesc()
		: Resource(InputLayoutDesc::kClassConcreteName)
	{}
	InputLayoutDesc::~InputLayoutDesc()
	{}
	ComPtr<ID3D11InputLayout> InputLayoutDesc::CreateInputLayout( s_ptr<VertexShader> vs)
	{
		ComPtr<ID3D11InputLayout> input_layout = nullptr;
		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateInputLayout(desc_.data(),
			static_cast<UINT>(desc_.size()),
			vs->GetByteCode().data(),
			vs->GetByteCode().size(),
			input_layout.GetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return input_layout;
	}
}