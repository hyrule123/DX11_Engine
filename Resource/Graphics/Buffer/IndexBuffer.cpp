#include "Engine/Core/pch.h"
#include "IndexBuffer.h"

#include <Engine/Core/Debug.h>

namespace engine
{
	IndexBuffer::IndexBuffer()
		: Resource(STRINGIFY(IndexBuffer))
	{
	}
	IndexBuffer::~IndexBuffer()
	{}
	bool IndexBuffer::Create(ID3D11Device* device, const std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (indices.empty())
		{
			ASSERT_MESSAGE(false, "인덱스가 저장된 포인터가 nullptr입니다.");
			return false;
		}

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)(sizeof(UINT) * indices.size());
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		//인덱스 버퍼 데이터 복사
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices.data();

		HRESULT hr = device->CreateBuffer(&desc, &data, index_buffer_.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			HRESULT_ERROR_MESSAGE(hr);
			return false;
		}

		indices_ = indices;
		topology_ = topology;

		return true;
	}

	void IndexBuffer::Bind(ID3D11DeviceContext* context)
	{
		context->IASetPrimitiveTopology(topology_);
		context->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}