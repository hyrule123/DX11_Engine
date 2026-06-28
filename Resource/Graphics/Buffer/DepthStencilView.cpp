#include "Engine/Core/pch.h"
#include "DepthStencilView.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	DepthStencilView::DepthStencilView()
		: Super(STRINGIFY(DepthStencilView))
	{
	}
	DepthStencilView::~DepthStencilView()
	{
	}

    bool DepthStencilView::CreateDSV(ID3D11Device* device, D3D11_DEPTH_STENCIL_VIEW_DESC* dsv_desc)
    {
		auto tex = GetTexture2D();
		if (!tex)
		{
			ERROR_MESSAGE("Texture를 먼저 만드세요");
			return false;
		}

		HRESULT hr = device->CreateDepthStencilView(tex.Get(), dsv_desc, dsv_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

        return true;
    }
}