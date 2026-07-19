#include "Engine/Core/pch.h"
#include "DepthStencilState.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	DepthStencilState::DepthStencilState()
		: Super(STRINGIFY(DepthStencilState))
	{}
	DepthStencilState::~DepthStencilState()
	{}

	bool DepthStencilState::Create( const D3D11_DEPTH_STENCIL_DESC& desc)
	{
		auto device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateDepthStencilState(&desc, ds_state_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		return true;
	}

	void DepthStencilState::Bind(ID3D11DeviceContext * context, uint32 stencil_ref)
	{
		context->OMSetDepthStencilState(ds_state_.Get(), stencil_ref);
	}
}