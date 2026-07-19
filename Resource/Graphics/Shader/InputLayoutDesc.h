#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

namespace engine
{
	class VertexShader;
	class InputLayoutDesc :
		public Resource
	{
		CLASS_INFO(InputLayoutDesc, Resource)

	public:
		InputLayoutDesc();
		virtual ~InputLayoutDesc() override;

		bool IsReady() const { return !desc_.empty(); }

		void SetLayoutDesc(const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc) { desc_ = desc; }
		void AddLayoutDesc(const D3D11_INPUT_ELEMENT_DESC& desc) { desc_.push_back(desc); }

		const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetInputLayoutDesc() const { return desc_; }

		ComPtr<ID3D11InputLayout> CreateInputLayout( s_ptr<VertexShader> vs);

	private:
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc_ = {};
	};
}


