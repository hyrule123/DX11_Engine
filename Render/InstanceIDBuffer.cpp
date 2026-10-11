#include "Engine/Core/pch.h"
#include "InstanceIDBuffer.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Resource/GPU/Vertex.h>	// vertex::instance_id::kInputSlot

#include <Engine/Core/Debug.h>

#include <algorithm>
#include <numeric>
#include <vector>

namespace engine
{
	InstanceIDBuffer::InstanceIDBuffer() = default;
	InstanceIDBuffer::~InstanceIDBuffer() = default;

	void InstanceIDBuffer::Init(ID3D11DeviceContext* context)
	{
		CHECK(has_initialized_ == false);

		has_initialized_ = true;
		// 초기 용량 kMinCapacity로 생성
		const bool result = Recreate(kMinCapacity);
		CHECK(result);
		Bind(context);
	}

	bool InstanceIDBuffer::EnsureCapacity(ID3D11DeviceContext* context, uint32 count)
	{
		if (buffer_ != nullptr && count <= capacity_)
		{
			return true;
		}

		// 1.5배, 그래도 부족하면 요청량만큼 (InstanceBufferPool과 동일 정책)
		const uint32 grown = capacity_ + capacity_ / 2;
		const uint32 new_capacity = std::max({ count, grown, kMinCapacity });

		if (false == Recreate(new_capacity))
		{
			return false;
		}

		// 기존 버퍼가 바인딩되어 있었다면 새 버퍼로 교체
		Bind(context);
		return true;
	}

	void InstanceIDBuffer::Bind(ID3D11DeviceContext* context) const
	{
		ASSERT_F(buffer_ != nullptr, "InstanceIDBuffer가 생성되지 않았습니다.");

		const UINT stride = sizeof(uint32);
		const UINT offset = 0;
		context->IASetVertexBuffers(vertex::instance_id::kInputSlot, 1, buffer_.GetAddressOf(), &stride, &offset);
	}

	bool InstanceIDBuffer::Recreate(uint32 new_capacity)
	{
		// 0, 1, 2, ..., new_capacity - 1
		std::vector<uint32> ids(new_capacity);
		std::iota(ids.begin(), ids.end(), 0u);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)(sizeof(uint32) * new_capacity);
		desc.Usage = D3D11_USAGE_IMMUTABLE;			// 생성 후 내용 불변
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 일반 정점 버퍼와 동일
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = ids.data();				// IMMUTABLE은 초기 데이터 필수

		ComPtr<ID3D11Buffer> new_buffer = {};
		const HRESULT result = GraphicsDevice::GetInst().GetDevice()->CreateBuffer(&desc, &init_data, new_buffer.GetAddressOf());
		if (FAILED(result))
		{
			ERR_MSG_HRESULT(result);
			return false;
		}

		// 실패 시 기존 버퍼 유지를 위해 성공 후에만 교체
		buffer_ = std::move(new_buffer);
		capacity_ = new_capacity;
		return true;
	}
}