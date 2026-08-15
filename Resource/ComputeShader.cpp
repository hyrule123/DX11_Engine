#include "Engine/Core/pch.h"
#include "ComputeShader.h"

#include <Engine/Util/file_system.h>
#include <Engine/Core/Debug.h>

#include <Engine/Manager/GraphicsDevice.h>

namespace engine
{
	ComputeShader::ComputeShader()
		: Super(ComputeShader::kClassConcreteName)
	{}
	ComputeShader::~ComputeShader()
	{}
	bool ComputeShader::Create(const std::vector<uint8>& bytecode)
	{
		auto* device = GraphicsDevice::GetInst().GetDevice();

		HRESULT hr = device->CreateComputeShader(bytecode.data(), bytecode.size(), nullptr, compute_shader_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		// --- numthreads 추출 ---
		ComPtr<ID3D11ShaderReflection> reflector;
		hr = ::D3DReflect(bytecode.data(), bytecode.size(),
			IID_ID3D11ShaderReflection,
			reinterpret_cast<void**>(reflector.GetAddressOf()));
		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		UINT x = 0, y = 0, z = 0;
		reflector->GetThreadGroupSize(&x, &y, &z);
		thread_group_size_ = { x, y, z };

		return true;
	}
	void ComputeShader::Bind(ID3D11DeviceContext* context)
	{
		context->CSSetShader(compute_shader_.Get(), nullptr, 0);
	}
	void ComputeShader::Unbind(ID3D11DeviceContext* context)
	{
		context->CSSetShader(nullptr, nullptr, 0);
	}
}