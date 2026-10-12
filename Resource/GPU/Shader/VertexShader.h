#pragma once
#include "Shader.h"

#include <Engine/Core/UtilMacro.h>



namespace engine
{
	class VertexShader final
		: public Shader
	{
		ENTITY_INFO(VertexShader, Shader);
	public:
		VertexShader();
		virtual ~VertexShader() final;

		bool IsReady() const { return (bool)vs_; }

		virtual void Bind(ID3D11DeviceContext* context) const final;

		const std::vector<uint8>& GetByteCode() const { return byte_code_; }

		// VS 입력 시그니처에 INSTANCE_ID 시맨틱이 있는지 (로드 시 리플렉션으로 판정)
		bool UsesInstanceID() const { return uses_instance_id_; }

	protected:
		virtual bool Create(const std::vector<uint8>& bytecode) final;

	private:
		bool ReflectInputSignature(const std::vector<uint8>& bytecode);

	private:
		ComPtr<ID3D11VertexShader> vs_ = {};
		std::vector<uint8> byte_code_ = {};

		bool uses_instance_id_ = false;
	};
}

