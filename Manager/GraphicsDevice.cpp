#include "Engine/Core/pch.h"
#include "GraphicsDevice.h"

#include <Engine/Manager/RenderManager.h>

#include <Engine/Resource/GraphicsPipeline/RenderTarget.h>

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
		swap_chain_RT_ = nullptr;
		
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

		//RTV 생성
		ComPtr<ID3D11RenderTargetView> rtv = CreateSwapChainRTV(swap_chain_, resolution_width, resolution_height);
		if (nullptr == rtv)
		{
			ASSERT_RELEASE_MESSAGE(false, "Failed to create Render Target View!");
			return false;
		}

		//DSV 생성
		ComPtr<ID3D11DepthStencilView> dsv = CreateSwapChainDSV(resolution_width, resolution_height);
		if (nullptr == dsv)
		{
			ASSERT_RELEASE_MESSAGE(false, "Failed to create Render Target View!");
			return false;
		}

		//Wrapper class에 저장
		swap_chain_RT_ = std::make_shared<RenderTargetGroup>();
		RenderTargetGroup::RTVArray views = { rtv, nullptr, };
		swap_chain_RT_->SetRenderTargets(views);
		swap_chain_RT_->SetDepthStencilView(dsv);

		//Depth Stencil State 생성
		depth_stencil_state_ = CreateSwapChainDSS();

		//Viewport 생성
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

		RenderManager::GetInst().OnResolutionChange(resolution_width, resolution_height);

		return true;
	}

	void GraphicsDevice::BindSwapChainRTV()
	{
		context_->OMSetDepthStencilState(depth_stencil_state_.Get(), 1);
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
	ComPtr<ID3D11RenderTargetView> GraphicsDevice::CreateSwapChainRTV(ComPtr<IDXGISwapChain> swap_chain, uint32 width, uint32 height)
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
	ComPtr<ID3D11DepthStencilView> GraphicsDevice::CreateSwapChainDSV(uint32 width, uint32 height)
	{
		// -----------------------------------------------------------------
	// STEP 1. 깊이와 스텐실 값을 저장할 알맹이(2D 텍스처 리소스) 생성
	// -----------------------------------------------------------------
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;            // RTV의 가로 크기와 완전히 일치해야 합니다.
		descDepth.Height = height;          // RTV의 세로 크기와 완전히 일치해야 합니다.
		descDepth.MipLevels = 1;               // 깊이 버퍼는 밉맵이 필요 없으므로 1 고정입니다.
		descDepth.ArraySize = 1;

		// Reversed-Z 사용 시 f32 포맷 사용
		// 차후 Stencil 버퍼 필요 시 버퍼 공간을 확장
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		// 멀티샘플링 설정 (RTV 생성할 때 넣은 Count, Quality 값과 무조건 일치해야 에러가 안 납니다)
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;

		descDepth.Usage = D3D11_USAGE_DEFAULT;          // GPU가 매 프레임 읽고 쓸 것이므로 DEFAULT
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 이 텍스처의 용도는 깊이/스텐실 도화지임을 명시
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		// GPU 메모리에 실제 깊이/스텐실 버퍼 공간 할당
		ComPtr<ID3D11Texture2D> ds_buffer = {};
		HRESULT hr = device_->CreateTexture2D(&descDepth, nullptr, ds_buffer.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		// -----------------------------------------------------------------
		// STEP 2. 생성된 버퍼를 파이프라인에 바인딩하기 위한 사양서(View) 생성
		// -----------------------------------------------------------------
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = descDepth.Format; // 위에서 지정한 포맷 구조를 그대로 따릅니다.

		// 이 리소스를 단순한 2D 텍스처 형태로 다루겠다고 설정
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0; // 원본 이미지 레벨(0번)을 조준

		ComPtr<ID3D11DepthStencilView> dsv = {};
		// 최종적으로 파이프라인 출력 병합기(OM)에 꽂을 DSV 객체 생성
		hr = device_->CreateDepthStencilView(ds_buffer.Get(), &descDSV, dsv.GetAddressOf());
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		return dsv;
	}
	ComPtr<ID3D11DepthStencilState> GraphicsDevice::CreateSwapChainDSS()
	{
		// 1. Depth Stencil Desc 구조체 선언 및 초기화
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};

		// --- DEPTH (Reversed-Z 핵심 설정) ---
		dsDesc.DepthEnable = TRUE;                           // 깊이 테스트 활성화
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 버퍼 쓰기 활성화

		// ★ Reversed-Z의 핵심: LESS_EQUAL 대신 GREATER_EQUAL을 사용합니다.
		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

		// --- STENCIL (기본 마스킹 설정 - 필요에 따라 수정 가능) ---
		dsDesc.StencilEnable = FALSE;                         // 스텐실 테스트 비활성화

		// 2. State 객체 생성
		ComPtr<ID3D11DepthStencilState> depth_stencil_state = nullptr;
		HRESULT hr = device_->CreateDepthStencilState(&dsDesc, &depth_stencil_state);

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return nullptr;
		}

		if (SUCCEEDED(hr)) {
			// 렌더링 컨텍스트에 바인딩 (스텐실 Ref 값은 1로 예시 설정)
			context_->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
		}

		return depth_stencil_state;
	}
}