#include "Engine/Core/pch.h"
#include "GraphicsDevice.h"

#include <Engine/Core/EngineMain.h>
#include <Engine/Core/Constant.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/DX11.h>

namespace engine
{
	GraphicsDevice::GraphicsDevice()
	{
	}

	GraphicsDevice::~GraphicsDevice()
	{
	}

	bool GraphicsDevice::Init()
	{
		bool result = CreateDeviceAndContext();
		if (false == result)
		{
			ASSERT_RELEASE(false);
			return false;
		}

		result = SetResolution(kDefaultWidth, kDefaultHeight);
		if (false == result)
		{
			ASSERT_RELEASE(false);
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateDeviceAndContext()
	{
		// Device, Device Context
		UINT DeviceFlag{};
#ifdef _DEBUG
		DeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL FeatureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_3,
			//D3D_FEATURE_LEVEL_9_2,
			//D3D_FEATURE_LEVEL_9_1
		};

		HRESULT result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			DeviceFlag,
			FeatureLevels,
			sizeof(FeatureLevels) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,
			&device_,
			&MaxSupportedFeatureLevel,
			&context_);

		if (FAILED(result))
		{
			HRESULT_ERROR_MESSAGE(result);
			return false;
		}

		return true;
	}
	bool GraphicsDevice::SetResolution(uint32 resolution_width, uint32 resolution_height)
	{
		HWND hwnd = EngineMain::GetInst().GetHWND();

		HRESULT hr = E_FAIL;

		//변경 전 초기화
		context_->ClearState();
		swap_chain_RTV.Reset();

		//스왑체인 생성
		if (nullptr == swap_chain_)
		{
			swap_chain_ = CreateSwapChain(hwnd, resolution_width, resolution_height);
			if (nullptr == swap_chain_)
			{
				ASSERT_RELEASE_MESSAGE(false, "Failed to create Swap Chain!");
				return false;
			}
		}
		else
		{
			hr = swap_chain_->ResizeBuffers(0, resolution_width, resolution_height, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
			{
				HRESULT_ERROR_MESSAGE(hr);
				return false;
			}
		}

		swap_chain_RTV = CreateSwapChainRenderTargetView(swap_chain_, resolution_width, resolution_height);
		if (nullptr == swap_chain_RTV)
		{
			ASSERT_RELEASE_MESSAGE(false, "Failed to create Render Target View!");
			return false;
		}

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)resolution_width;
		viewport.Height = (float)resolution_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context_->RSSetViewports(1, &viewport);

		resolution_width_ = resolution_width;
		resolution_height_ = resolution_height;

		return true;
	}

	void GraphicsDevice::BindSwapChainRTV()
	{
		// 3. 파이프라인의 출력 병합기(Output Merger) 단계에 렌더 타겟을 바인딩합니다.
		// 아직 깊이/스텐실 버퍼(Depth/Stencil)가 없다면 두 번째 인자는 nullptr로 둡니다.
		context_->OMSetRenderTargets(1, swap_chain_RTV.GetAddressOf(), nullptr);
	}

	void GraphicsDevice::Render()
	{ 
		swap_chain_->Present(1, 0);
	}

	void GraphicsDevice::FrameEnd()
	{
		// 1. 렌더 타겟 배경색으로 지우기
		const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		context_->ClearRenderTargetView(swap_chain_RTV.Get(), clearColor);

		// 2. 깊이/스텐실 버퍼 초기화
		//context_->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	ComPtr<IDXGISwapChain> GraphicsDevice::CreateSwapChain(HWND hwnd, uint32 width, uint32 height)
	{
		if (nullptr == device_ || NULL == hwnd)
		{
			ERROR_MESSAGE("D3D11 Device is not created yet!");
			return nullptr;
		}

		HRESULT hr = E_FAIL;

		// 1. D3D11 디바이스에서 DXGI 디바이스 인터페이스를 얻어옵니다.
		ComPtr<IDXGIDevice> pDXGIDevice = {};
		hr = device_->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
		if (FAILED(hr)) 
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr; 
		}

		// 2. DXGI 디바이스에서 그래픽 카드(어댑터) 정보를 얻어옵니다.
		ComPtr<IDXGIAdapter> pDXGIAdapter = {};
		hr = pDXGIDevice->GetAdapter(pDXGIAdapter.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		// 3. 어댑터에서 최종적으로 DXGI 팩토리를 얻어옵니다.
		ComPtr<IDXGIFactory> pDXGIFactory = {};
		hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pDXGIFactory.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		// 4. 스왑 체인 설정
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 2;                         // 후면 버퍼 2개
		sd.BufferDesc.Width = width;                // 해상도 너비
		sd.BufferDesc.Height = height;              // 해상도 높이
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 픽셀 포맷
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;                     // 출력할 윈도우 핸들!
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;                         // 창 모드

		//DXGI WARNING: IDXGIFactory::CreateSwapChain: Blt-model swap effects (DXGI_SWAP_EFFECT_DISCARD and DXGI_SWAP_EFFECT_SEQUENTIAL) are legacy swap effects that are predominantly superceded by their flip-model counterparts (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD)
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		// 5. 얻어낸 DXGI 팩토리를 이용해 스왑 체인 생성
		// 첫 번째 인자로 팩토리가 아니라 D3D11 디바이스를 넘겨주는 것에 주의하세요.
		ComPtr<IDXGISwapChain> swap_chain = {};
		hr = pDXGIFactory->CreateSwapChain(device_.Get(), &sd, swap_chain.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return swap_chain;
	}
	ComPtr<ID3D11RenderTargetView> GraphicsDevice::CreateSwapChainRenderTargetView(ComPtr<IDXGISwapChain> swap_chain, uint32 width, uint32 height)
	{
		// 1. 스왑 체인으로부터 백 버퍼(Texture2D) 포인터를 가져옵니다.
		ComPtr<ID3D11Texture2D> backBuffer;
		HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		// 2. 백 버퍼 정보를 바탕으로 Render Target View를 생성합니다.
		// 팁: 두 번째 인자(Desc)에 nullptr을 넣으면 백 버퍼의 리소스 포맷을 그대로 상속받습니다.
		ComPtr<ID3D11RenderTargetView> rtv;
		hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, rtv.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return rtv;
	}
}