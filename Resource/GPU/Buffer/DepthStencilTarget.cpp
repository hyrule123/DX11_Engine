#include "Engine/Core/pch.h"
#include "DepthStencilTarget.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	DepthStencilTarget::DepthStencilTarget()
		: Super(DepthStencilTarget::kClassConcreteName)
	{
	}
	DepthStencilTarget::~DepthStencilTarget()
	{
	}

    bool DepthStencilTarget::CreateDSV( D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc)
    {
		auto tex = GetTexture2D();
		if (!tex)
		{
			ERR_MSG("Texture를 먼저 만드세요");
			return false;
		}

		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateDepthStencilView(tex.Get(), dsv_desc, dsv_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

        return true;
    }
	bool DepthStencilTarget::Resize(uint32_t width, uint32_t height)
	{
		bool result = Super::Resize(width, height);
		if (!result)
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
		dsv_->GetDesc(&dsv_desc);

		return CreateDSV(&dsv_desc);
	}
}