#include "Engine/Core/pch.h"
#include "RasterizerState.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	RasterizerState::RasterizerState()
		: Super(RasterizerState::kClassConcreteName)
	{
	}
	RasterizerState::~RasterizerState()
	{
	}
	bool RasterizerState::Create( const D3D11_RASTERIZER_DESC& desc)
	{
		ComPtr<ID3D11RasterizerState> raw_rss = {};

		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateRasterizerState(&desc, raw_rss.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		return true;
	}
	void RasterizerState::Bind(ID3D11DeviceContext* context)
	{
		context->RSSetState(rasterizer_state_.Get());
	}
}