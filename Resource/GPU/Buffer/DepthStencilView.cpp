#include "Engine/Core/pch.h"
#include "DepthStencilView.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	DepthStencilView::DepthStencilView()
		: Super(DepthStencilView::kClassConcreteName)
	{
	}
	DepthStencilView::~DepthStencilView()
	{
	}

    bool DepthStencilView::CreateDSV( D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc)
    {
		auto tex = GetTexture2D();
		if (!tex)
		{
			ERROR_MESSAGE("Texture를 먼저 만드세요");
			return false;
		}

		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateDepthStencilView(tex.Get(), dsv_desc, dsv_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

        return true;
    }
	bool DepthStencilView::Resize(uint32 width, uint32 height)
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