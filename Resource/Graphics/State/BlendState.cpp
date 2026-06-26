#include "Engine/Core/pch.h"
#include "BlendState.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	BlendState::BlendState()
		: Super(STRINGIFY(BlendState))
	{}
	BlendState::~BlendState()
	{}
	bool BlendState::Create(ID3D11Device * device, const D3D11_BLEND_DESC & desc)
	{
		HRESULT hr = device->CreateBlendState(&desc, blend_state_.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		return true;
	}
	void BlendState::Bind(ID3D11DeviceContext* context, const float* blend_factor, uint32 sample_mask)
	{
		context->OMSetBlendState(blend_state_.Get(), blend_factor, 0xFFFFFFFF);
	}
}