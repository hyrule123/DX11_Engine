#pragma once
#include "Shader.h"

#include <Engine/Core/UtilMacro.h>

namespace engine
{
	class VertexShader final
		: public Shader
	{
		CLASS_INFO(VertexShader, Shader)
	public:
		VertexShader();
		virtual ~VertexShader() final;

		virtual void Bind(const ComPtr<ID3D11DeviceContext>& context) final;

		const std::vector<uint8>& GetByteCode() const { return byte_code_; }

	protected:
		virtual bool Create(const ComPtr<ID3D11Device>& device, const std::vector<uint8>& bytecode) final;

	private:
		ComPtr<ID3D11VertexShader> vs_ = {};
		std::vector<uint8> byte_code_ = {};
	};
}

