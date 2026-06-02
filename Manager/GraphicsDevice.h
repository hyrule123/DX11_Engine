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

	public:
		bool Init();

	private:
		ComPtr<ID3D11Device> device_ = {};
		ComPtr<ID3D11DeviceContext> context_ = {};
		ComPtr<IDXGISwapChain> swap_chain_ = {};
		ComPtr<ID3D11RenderTargetView> render_target_view_ = {};
	};
}


