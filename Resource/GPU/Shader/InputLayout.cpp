#include "Engine/Core/pch.h"
#include "InputLayout.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Resource/GPU/Shader/VertexShader.h>

namespace engine
{
	InputLayout::InputLayout()
		: Super(InputLayout::kClassConcreteName)
	{}
	InputLayout::~InputLayout()
	{}
	bool InputLayout::Create(std::span<const D3D11_INPUT_ELEMENT_DESC> layout_descs, VertexShader* vs)
	{
		if (layout_descs.empty())
		{
			ERR_MSG("InputLayoutDesc is empty.");
			return false;
		}
		if (vs == nullptr || vs->IsReady() == false)
		{
			ERR_MSG("VertexShader is not ready.");
			return false;
		}

		auto* device = GraphicsDevice::GetInst().GetDevice();

		HRESULT hr = device->CreateInputLayout(
			layout_descs.data(),
			static_cast<UINT>(layout_descs.size()),
			vs->GetByteCode().data(),
			vs->GetByteCode().size(),
			input_layout_.GetAddressOf()
		);

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		return true;
	}
	void InputLayout::Bind(ID3D11DeviceContext* context)
	{
		ASSERT(input_layout_ != nullptr);
		context->IASetInputLayout(input_layout_.Get());
	}
}