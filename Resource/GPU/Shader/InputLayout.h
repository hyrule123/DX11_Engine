#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/DX11.h>

#include <span>

namespace engine
{
	class VertexShader;

	class InputLayout :
		public Resource
	{
		ENTITY_INFO(InputLayout, Resource);

	public:
		InputLayout();
		virtual ~InputLayout() override;

		bool Create(std::span<const D3D11_INPUT_ELEMENT_DESC> layout_descs, VertexShader* vs);

		void Bind(ID3D11DeviceContext* context) const;

	private:
		ComPtr<ID3D11InputLayout> input_layout_;
	};
}


