#include "Engine/Core/pch.h"
#include "RenderTargetView.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	RenderTargetView::RenderTargetView()
		: Super(STRINGIFY(RenderTargetView))
	{}
	RenderTargetView::~RenderTargetView()
	{}
	bool RenderTargetView::CreateDefault(uint32 width, uint32 height, DXGI_FORMAT format)
	{
		// 내부에서 표준 DESC를 조립
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		bool tex_result = CreateTexture2D(&desc);
		if (!tex_result)
		{
			ERROR_MESSAGE("텍스처 생성 실패!!");
			return false;
		}

		return CreateRTV(nullptr);
	}
	bool RenderTargetView::CreateRTV(D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc)
	{
		auto tex = GetTexture2D();
		if (!tex)
		{
			ERROR_MESSAGE("Texture를 먼저 만드세요");
			return false;
		}

		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateRenderTargetView(tex.Get(), rtv_desc, render_target_view_.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		return true;
	}
	bool RenderTargetView::CreateForSwapchain( ComPtr<IDXGISwapChain> swap_chain)
	{
		// 1. 스왑 체인으로부터 백 버퍼(Texture2D) 포인터를 가져옵니다.
		ComPtr<ID3D11Texture2D> back_buffer;
		HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)back_buffer.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
		SetTexture2D(back_buffer);

		// 2. 백 버퍼 정보를 바탕으로 Render Target View를 생성합니다.
		// 팁: 두 번째 인자(Desc)에 nullptr을 넣으면 백 버퍼의 리소스 포맷을 그대로 상속받습니다.
		hr = GraphicsDevice::GetInst().GetDevice()->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_view_.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
			
		return true;
	}
	bool RenderTargetView::Resize( uint32 width, uint32 height)
	{
		bool result = Super::Resize(width, height);
		if (!result)
		{
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};
		render_target_view_->GetDesc(&rtv_desc);
		
		result = CreateRTV(&rtv_desc);
		ASSERT(result);
		return true;
	}
}