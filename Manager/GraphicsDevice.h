#pragma once

#include <Engine/Core/Common.h>
#include <Engine/Core/Singleton.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;

namespace engine
{
	class GraphicsDevice
	{
		DECLARE_SINGLETON(GraphicsDevice)

		friend class GameEngine;

	public:
		ComPtr<ID3D11Device> GetDevice() const { return device_; }
		ComPtr<ID3D11DeviceContext> GetContext() const { return context_; }

		bool SetResolution(uint32 resolution_width, uint32 resolution_height);

		void BindSwapChainRTV();
		void Render();

	private:
		bool Init();
		bool CreateDeviceAndContext();

		ComPtr<IDXGISwapChain> CreateSwapChain(HWND hwnd, uint32 width, uint32 height);
		ComPtr<ID3D11RenderTargetView> CreateSwapChainRenderTargetView(ComPtr<IDXGISwapChain> swap_chain, uint32 width, uint32 height);

	private:
		ComPtr<ID3D11Device> device_ = {};
		ComPtr<ID3D11DeviceContext> context_ = {};
		ComPtr<IDXGISwapChain> swap_chain_ = {};
		ComPtr<ID3D11RenderTargetView> swap_chain_RTV = {};

		uint32 resolution_width_ = {};
		uint32 resolution_height_ = {};
	};
}


