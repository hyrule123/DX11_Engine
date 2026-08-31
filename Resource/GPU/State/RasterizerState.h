#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>

struct ID3D11RasterizerState;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine
{
	
	class RasterizerState :
		public Resource
	{
		ENTITY_INFO(RasterizerState, Resource)
	public:
		RasterizerState();
		virtual ~RasterizerState() override;

		bool IsReady() const { return (bool)rasterizer_state_; }

		void SetRasterizerState(ComPtr<ID3D11RasterizerState> rss) {
			rasterizer_state_ = rss;
		}

		bool Create( const D3D11_RASTERIZER_DESC& desc);
		void Bind(ID3D11DeviceContext* context);

	private:
		ComPtr<ID3D11RasterizerState> rasterizer_state_ = {};
	};
}


