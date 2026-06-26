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

    bool DepthStencilView::Create(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc)
    {
        HRESULT hr = S_OK;

        if (false == (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
        {
            ERROR_MESSAGE("D3D11_BIND_DEPTH_STENCIL flag가 설정되지 않았습니다.");
            return false;
        }

        ComPtr<ID3D11Texture2D> tex = {};
        // 1. 텍스처 생성 (Desc 그대로 사용 - 이 때 desc.Format은 반드시 TYPELESS여야 함)
        hr = device->CreateTexture2D(&desc, nullptr, tex.GetAddressOf());
        if (FAILED(hr)) 
        { 
            HRESULT_ERROR_MESSAGE(hr);
            return false; 
        } 

        // TYPELESS 포맷을 보고 DSV와 SRV의 포맷을 짝지어 주기
        DXGI_FORMAT dsvFormat = desc.Format;
        DXGI_FORMAT srvFormat = desc.Format;

        switch (desc.Format)
        {
        case DXGI_FORMAT_R24G8_TYPELESS:
            dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            dsvFormat = DXGI_FORMAT_D32_FLOAT;
            srvFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            dsvFormat = DXGI_FORMAT_D32_FLOAT;
            if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
            {
                ERROR_MESSAGE("DXGI_FORMAT_D32_FLOAT 포맷은 SRV를 생성할 수 없습니다.");
                return false;
            }
            break;
        case DXGI_FORMAT_R16_TYPELESS: // 모바일이나 저해상도 그림자에 주로 쓰임
            dsvFormat = DXGI_FORMAT_D16_UNORM;
            srvFormat = DXGI_FORMAT_R16_UNORM;
            break;
        default:
            ERROR_MESSAGE("해당 포맷 처리 방법 없음. 추가 필요.");
            // 만약 사용자가 실수로 D24_UNORM_S8_UINT 같은 타입이 정해진 포맷을 넣었다면 
            // SRV를 만들 수 없으므로, 그냥 그대로 둔 채 아래에서 에러가 나도록 하거나 예외 처리
            break;
        }

        // 2. DSV 생성
        ComPtr<ID3D11DepthStencilView> dsv = {};
        if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = dsvFormat; // 짝맞춘 포맷 적용
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            hr = device->CreateDepthStencilView(tex.Get(), &dsvDesc, dsv.GetAddressOf());
            if (FAILED(hr)) 
            { 
                HRESULT_ERROR_MESSAGE(hr);
                return false; 
            }
        }

        // 3. SRV 생성 (옵션)
        ComPtr<ID3D11ShaderResourceView> srv = {};
        if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = srvFormat; // 짝맞춘 포맷 적용
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels; // MipLevels 동기화

            hr = device->CreateShaderResourceView(tex.Get(), &srvDesc, srv.GetAddressOf());
            if (FAILED(hr))
            {
                HRESULT_ERROR_MESSAGE(hr);
                return false;
            }
        }

        SetTexture2D(tex);
        SetShaderResourceView(srv);
        dsv_ = dsv;
        SetSize(desc.Width, desc.Height);

        return true;
    }
}