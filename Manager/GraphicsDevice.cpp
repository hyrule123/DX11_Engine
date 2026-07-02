#include "Engine/Core/pch.h"
#include "GraphicsDevice.h"

#include <Engine/Manager/RenderManager.h>

#include <Engine/Resource/Graphics/Buffer/RenderTargetView.h>
#include <Engine/Resource/Graphics/Buffer/DepthStencilView.h>
#include <Engine/Resource/Graphics/RenderTargetGroup.h>

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
	void GraphicsDevice::ClearContextStates()
	{
		context_->ClearState();
		RenderManager::GetInst().OnClearContextStates();
	}
	bool GraphicsDevice::SetResolution(uint32 resolution_width, uint32 resolution_height)
	{
		HWND hwnd = EngineMain::GetInst().GetHWND();

		HRESULT hr = E_FAIL;

		//변경 전 초기화
		ClearContextStates();
		if (swap_chain_RT_) { swap_chain_RT_->Reset(); }
		else { swap_chain_RT_ = std::make_shared<RenderTargetGroup>(); }
		
		if (nullptr == swap_chain_)
		{
			//스왑체인 새로 생성
			swap_chain_ = CreateSwapChain(hwnd, resolution_width, resolution_height);
			if (nullptr == swap_chain_)
			{
				ASSERT_RELEASE_MESSAGE(false, "Failed to create Swap Chain!");
				return false;
			}
		}
		else
		{
			//이미 생성된 경우 스왑체인 크기만 조절
			hr = swap_chain_->ResizeBuffers(0, resolution_width, resolution_height, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
			{
				HRESULT_ERROR_MESSAGE(hr);
				return false;
			}
		}

#pragma region RTV
		//RTV 생성
		s_ptr<RenderTargetView> rt = std::make_shared<RenderTargetView>();

		bool result = rt->CreateForSwapchain(device_.Get(), swap_chain_);
		if (!result)
		{
			ASSERT_RELEASE_MESSAGE(false, "Failed to create Render Target");
			return false;
		}
		swap_chain_RT_->SetRenderTargets({ rt, });
#pragma endregion RTV

#pragma region DSV
		//DSV 생성
		s_ptr<DepthStencilView> dsv = std::make_shared<DepthStencilView>();
		D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
		depth_buffer_desc.Width = resolution_width;            // RTV의 가로 크기와 완전히 일치해야 합니다.
		depth_buffer_desc.Height = resolution_height;          // RTV의 세로 크기와 완전히 일치해야 합니다.
		depth_buffer_desc.MipLevels = 1;               // 깊이 버퍼는 밉맵이 필요 없으므로 1 고정입니다.
		depth_buffer_desc.ArraySize = 1;

		// Reversed-Z 사용 시 f32 포맷 사용
		// 차후 Stencil 버퍼 필요 시 버퍼 공간을 확장
		depth_buffer_desc.Format = DXGI_FORMAT_D32_FLOAT;
		// 멀티샘플링 설정 (RTV 생성할 때 넣은 Count, Quality 값과 무조건 일치해야 에러가 안 납니다)
		depth_buffer_desc.SampleDesc.Count = 1;
		depth_buffer_desc.SampleDesc.Quality = 0;

		depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;          // GPU가 매 프레임 읽고 쓸 것이므로 DEFAULT
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 이 텍스처의 용도는 깊이/스텐실 도화지임을 명시
		depth_buffer_desc.CPUAccessFlags = 0;
		depth_buffer_desc.MiscFlags = 0;

		result = dsv->CreateTexture2D(device_.Get(), &depth_buffer_desc);
		ASSERT_RELEASE(result);

		result = dsv->CreateDSV(device_.Get(), nullptr);
		ASSERT_RELEASE(result);
#pragma endregion DSV

		swap_chain_RT_->SetDepthStencilView(dsv);

#pragma region Viewport
		//Viewport 생성
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)resolution_width;
		viewport.Height = (float)resolution_height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context_->RSSetViewports(1, &viewport);
#pragma endregion Viewport

		resolution_width_ = resolution_width;
		resolution_height_ = resolution_height;

		RenderManager::GetInst().OnResolutionChange(resolution_width, resolution_height);

		return true;
	}

	void GraphicsDevice::BindSwapChainRTV()
	{
		swap_chain_RT_->BindOutputMerger(context_.Get());
	}

	void GraphicsDevice::Present()
	{ 
		swap_chain_->Present(1, 0);
	}

	void GraphicsDevice::FrameEnd()
	{
		// 1. 렌더 타겟 배경색으로 지우기
		constexpr std::array<float, 4> color = { 0.1f, 0.1f, 0.1f, 1.0f };
		swap_chain_RT_->ClearRenderTargetView(context_.Get(), color);

		// 2. 깊이/스텐실 버퍼 초기화
		swap_chain_RT_->ClearDepthStencilView(context_.Get(), 0.0f, 0);
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
}