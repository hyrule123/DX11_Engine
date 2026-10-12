#include "Engine/Core/pch.h"
#include "VertexShader.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/GPU/Vertex.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

#pragma comment(lib, "dxguid.lib")

namespace engine
{
	VertexShader::VertexShader()
		: Shader(VertexShader::kClassConcreteName)
	{

	}
	VertexShader::~VertexShader()
	{
	}
	void VertexShader::Bind(ID3D11DeviceContext* context) const
	{
		context->VSSetShader(vs_.Get(), nullptr, 0);
	}
	bool VertexShader::Create( const std::vector<uint8>& bytecode)
	{
		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateVertexShader(bytecode.data(), bytecode.size(), nullptr, vs_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		if (false == ReflectInputSignature(bytecode))
		{
			return false;
		}

		byte_code_ = bytecode;

		return true;
	}
	bool VertexShader::ReflectInputSignature(const std::vector<uint8>& bytecode)
	{
		uses_instance_id_ = false;	// 재로드 대비 초기화

		ComPtr<ID3D11ShaderReflection> reflector;
		HRESULT hr = ::D3DReflect(bytecode.data(), bytecode.size(),
			IID_ID3D11ShaderReflection,
			reinterpret_cast<void**>(reflector.GetAddressOf()));
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		D3D11_SHADER_DESC shader_desc = {};
		hr = reflector->GetDesc(&shader_desc);
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		for (UINT i = 0; i < shader_desc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC param = {};
			hr = reflector->GetInputParameterDesc(i, &param);
			if (FAILED(hr))
			{
				ERR_MSG_HRESULT(hr);
				return false;
			}

			// HLSL 시맨틱은 대소문자를 구분하지 않는다
			if (_stricmp(param.SemanticName, vertex::instance_id::kSemanticName) != 0)
			{
				continue;
			}

			// 슬롯 31 desc는 INSTANCE_ID0 / R32_UINT 하나로 고정되어 있으므로 셰이더 선언도 이에 맞아야 한다
			ASSERT_F(param.SemanticIndex == 0, "INSTANCE_ID는 시맨틱 인덱스 0만 지원합니다.");
			ASSERT_F(param.ComponentType == D3D_REGISTER_COMPONENT_UINT32, "INSTANCE_ID는 uint로 선언해야 합니다.");
			ASSERT_F(param.Mask == 0x1, "INSTANCE_ID는 스칼라(uint)여야 합니다.");

			uses_instance_id_ = true;
		}

		return true;
	}
	
}