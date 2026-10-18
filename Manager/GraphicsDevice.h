#pragma once

#include <Engine/Core/Singleton.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;

namespace engine
{
	class RenderTargetGroup;

	class GraphicsDevice
	{
		DECLARE_SINGLETON(GraphicsDevice)

		friend class GameEngine;

	public:
		ID3D11Device* GetDevice() const { return device_.Get(); }
		ID3D11DeviceContext* GetContext() const { return context_.Get(); }

		void OnScreenSizeChange(uint32_t width, uint32_t height);
		bool SetResolution(uint32_t resolution_width, uint32_t resolution_height);
		uint32_t GetResolutionWidth() const { return resolution_width_; }
		uint32_t GetResolutionHeight() const { return resolution_height_; }

		void BindSwapChainRTV();
		void Present();
		void FrameEnd();

		RenderTargetGroup* GetSwapChainRenderTargetGroup() const { return swap_chain_RT_.get(); }

	private:
		bool Init();
		bool CreateDeviceAndContext();
		void ClearContextStates();

		ComPtr<IDXGISwapChain> CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height);
	private:
		ComPtr<ID3D11Device> device_ = {};
		ComPtr<ID3D11DeviceContext> context_ = {};

		ComPtr<IDXGISwapChain> swap_chain_ = {};

		std::unique_ptr<RenderTargetGroup> swap_chain_RT_ = {};

		uint32_t resolution_width_ = {};
		uint32_t resolution_height_ = {};

		D3D_FEATURE_LEVEL feature_level_ = {};
		bool rovs_supported_ = false;
		bool typed_uav_load_additional_formats_ = false;
	};
}


