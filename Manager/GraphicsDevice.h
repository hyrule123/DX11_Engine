#pragma once

#include <Engine/Common.h>
#include <Engine/Singleton.h>

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
		bool SetResolution(uint32 width, uint32 height);

	private:
		bool Init();
		bool CreateDeviceAndContext();
		bool CreateSwapChain(HWND hwnd, uint32 width, uint32 height);

	private:
		ComPtr<ID3D11Device> device_ = {};
		ComPtr<ID3D11DeviceContext> context_ = {};
		ComPtr<IDXGISwapChain> swap_chain_ = {};
		ComPtr<ID3D11RenderTargetView> swap_chain_RTV = {};

		uint32 width_ = {};
		uint32 height_ = {};
	};
}


