
#include "Engine/Core/pch.h"
#include "VertexBuffer.h"

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	VertexBuffer::VertexBuffer()
		: Resource(STRINGIFY(VertexBuffer))
	{
	}
	VertexBuffer::~VertexBuffer()
	{
	}
	bool VertexBuffer::Create(ID3D11Device* device, const void* data, UINT data_stride, UINT	data_count)
	{
		if (nullptr == data)
		{
			return false;
		}

		D3D11_BUFFER_DESC desc = {};
		// 버텍스 버퍼
		desc.ByteWidth = data_stride * data_count;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub_data = {};
		sub_data.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&desc, &sub_data, buffer_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		data_stride_ = data_stride;
		data_count_ = data_count;
		data_.clear();

		//정점 데이터를 memcpy를 통해서 복사
		data_.resize((size_t)desc.ByteWidth);
		memcpy(data_.data(), data, desc.ByteWidth);

		return true;
	}
	bool VertexBuffer::LoadFromFile(const stdfs::path& path)
	{
		//아직은... 미구현(차후 fbx 임포트 시 구현 예정)
		return false;
	}

	void VertexBuffer::Bind(ID3D11DeviceContext* context)
	{
		uint32_t offset = 0;
		// 0번 슬롯에 1개의 버퍼를 바인딩합니다.
		context->IASetVertexBuffers(0, 1, buffer_.GetAddressOf(), &data_stride_, &offset);
	}
}
