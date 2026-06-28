#include "Engine/Core/pch.h"
#include "ConstantBuffer.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	ConstantBuffer::ConstantBuffer()
		: Super(STRINGIFY(ConstantBuffer))
	{}
	ConstantBuffer::~ConstantBuffer()
	{}
	bool ConstantBuffer::Create(ID3D11Device* device, size_t stride)
	{
		if (stride == 0)
		{
			ERROR_MESSAGE("stride가 0입니다.");
			return false;
		}
		if ((stride % 16) != 0)
		{
			ERROR_MESSAGE("Constant Buffer에 사용할 데이터는 반드시 16바이트 정렬이 되어 있어야 합니다.");
			return false;
		}

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)stride;                            // 정렬된 크기 대입
		desc.Usage = D3D11_USAGE_DYNAMIC;                   // CPU에서 매 프레임 쓰기 가능하도록 설정
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// 상수 버퍼로 지정
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;       // Map(D3D11_MAP_WRITE_DISCARD) 권한 획득
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;                       // Constant Buffer는 0으로 둡니다.

		HRESULT hr = GraphicsDevice::GetInst().GetDevice()->CreateBuffer(&desc, nullptr, buffer_.GetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}
		
		stride_ = stride;

		return true;
	}

	void ConstantBuffer::Upload(ID3D11DeviceContext* context, const void* ptr, size_t size)
	{
		if (!ptr) 
		{
			DEBUG_LOG("NULLPTR!!!!!!!!!");
			return; 
		}
		if (size == 0)
		{
			DEBUG_LOG("Size is 0");
			return;
		}
		if (size != stride_)
		{
			DEBUG_LOG("Size unmatched!!!!!");
			return;
		}

		D3D11_MAPPED_SUBRESOURCE mapped{};

		HRESULT hr = context->Map(
			buffer_.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mapped);

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return;
		}

		memcpy(mapped.pData, ptr, size);

		context->Unmap(buffer_.Get(), 0);
	}

	void ConstantBuffer::Bind(ID3D11DeviceContext* context, ShaderStageFlag flag, UINT slot)
	{
		if (flag & ShaderStage::kVS)
		{
			context->VSSetConstantBuffers(slot, 1u, buffer_.GetAddressOf());
		}
		if (flag & ShaderStage::kGS)
		{
			context->GSSetConstantBuffers(slot, 1u, buffer_.GetAddressOf());
		}
		if (flag & ShaderStage::kPS)
		{
			context->PSSetConstantBuffers(slot, 1u, buffer_.GetAddressOf());
		}
		if (flag & ShaderStage::kCS)
		{
			context->CSSetConstantBuffers(slot, 1u, buffer_.GetAddressOf());
		}
	}
}