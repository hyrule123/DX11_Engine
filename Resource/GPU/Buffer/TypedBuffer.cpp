#include "Engine/Core/pch.h"
#include "TypedBuffer.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>
#include <Engine/Core/Debug.h>

#include <limits>

namespace engine
{
	TypedBuffer::TypedBuffer()
		: Resource(TypedBuffer::kClassConcreteName)
	{}
	TypedBuffer::~TypedBuffer()
	{}

	bool TypedBuffer::CreateImmutableBuffer(DXGI_FORMAT format, uint32 elem_stride, const void* data, uint32 count)
	{
		// IMMUTABLE은 초기 데이터 필수
		if (nullptr == data) 
		{ 
			ERR_MSG("data가 nullptr입니다. IMMUTABLE은 초기 데이터가 필수입니다.");
			return false; 
		}   
		if(GetDXGIFormatByteStride(format) != (UINT)elem_stride)
		{
			ERR_MSG("stride가 버퍼 포맷과 불일치합니다.");
			return false;
		}
		return CreateBufferImpl(format, count, D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE, 0, data);
	}

	bool TypedBuffer::CreateDynamicBuffer(DXGI_FORMAT format, uint32 capacity)
	{
		return CreateBufferImpl(format, capacity, D3D11_USAGE_DYNAMIC,
			D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, nullptr);
	}

	bool TypedBuffer::CreateDefaultBuffer(DXGI_FORMAT format, uint32 capacity, bool need_uav)
	{
		return CreateBufferImpl(format, capacity, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | (need_uav ? D3D11_BIND_UNORDERED_ACCESS : 0u),
			0, nullptr);
	}

	bool TypedBuffer::Upload(ID3D11DeviceContext* context, const void* data, uint32 elem_stride, uint32 elem_count)
	{
		if (data == nullptr)
		{
			ERR_MSG("data가 nullptr입니다.");
			return false;
		}
		if (elem_stride != stride_)
		{
			ERR_MSG("stride가 버퍼 포맷과 불일치합니다.");
			return false;
		}
		if (elem_count > capacity_)
		{
			ERR_MSG("capacity를 초과했습니다.");
			return false;
		}

		switch (buffer_usage_)
		{
		case D3D11_USAGE_IMMUTABLE:
		{
			ERR_MSG("Immutable 버퍼는 업로드할 수 없습니다.");
			return false;
		}
		case D3D11_USAGE_DEFAULT:
		{
			//참고: UpdateSubresource는 전체 갱신만 지원. 
			//Box의는 16 byte 단위로 정렬되어야 하는 제약이 있음. 따라서 Default 버퍼는 전체 갱신만 지원하도록 제한.
			if (elem_count != capacity_)
			{
				ERR_MSG("Default 버퍼는 전체 갱신만 지원합니다.");
				return false;
			}
			context->UpdateSubresource(buffer_.Get(), 0, nullptr, data,
				static_cast<UINT>(stride_ * capacity_), 0);
			break;
		}
		case D3D11_USAGE_DYNAMIC:
		{
			// Dynamic 버퍼는 내부 데이터를 '전부 버림'
			D3D11_MAPPED_SUBRESOURCE mapped = {};
			if (FAILED(context->Map(buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				ERR_MSG("Map 실패");
				return false;
			}
			std::memcpy(mapped.pData, data, elem_stride * elem_count);
			context->Unmap(buffer_.Get(), 0);
			break;
		}
		default:
			ERR_MSG("지원되지 않는 BufferUsage입니다.");
			return false;
		}

		count_ = elem_count;

		return true;
	}

	bool TypedBuffer::Reserve(uint32 new_capacity)
	{
		if (buffer_ == nullptr)
		{
			ERR_MSG("버퍼가 생성되지 않았습니다.");
			return false;
		}
		if (buffer_usage_ == D3D11_USAGE_IMMUTABLE)
		{
			ERR_MSG("Immutable 버퍼는 재할당할 수 없습니다.");
			return false;
		}
		// 이미 충분한 경우 return
		if (new_capacity <= capacity_) { return true; }

		D3D11_BUFFER_DESC desc = {};
		buffer_->GetDesc(&desc);

		desc.ByteWidth = stride_ * new_capacity;   // 이것만 바꿈

		auto* device = GraphicsDevice::GetInst().GetDevice();
		ComPtr<ID3D11Buffer> buffer;

		HRESULT hr = device->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		ComPtr<ID3D11ShaderResourceView> srv;
		ComPtr<ID3D11UnorderedAccessView> uav;

		// SRV와 UAV가 생성되어 있을 경우 재생성
		if (SRV_)
		{
			srv = CreateSRV(buffer.Get(), format_, new_capacity);
			if (!srv) { return false; }
		}
		if (UAV_)
		{
			uav = CreateUAV(buffer.Get(), format_, new_capacity);
			if (!uav) { return false; }
		}

		buffer_ = buffer;
		SRV_ = srv;
		UAV_ = uav;
		capacity_ = new_capacity;
		count_ = 0;
		return true;
	}

	void TypedBuffer::BindSRV(ID3D11DeviceContext* context, ShaderStage::Flags stage_flag, RegisterT slot)
	{
		ASSERT(context);
		ASSERT(SRV_);

		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Vertex))
		{
			context->VSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Geometry))
		{
			context->GSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Pixel))
		{
			context->PSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
		if (ShaderStage::HasFlag(stage_flag, ShaderStage::Flags::Compute))
		{
			context->CSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
	}

	void TypedBuffer::BindUAV(ID3D11DeviceContext * context, RegisterT slot)
	{
		ASSERT(context);
		ASSERT(UAV_);

		context->CSSetUnorderedAccessViews(slot.Get(), 1, UAV_.GetAddressOf(), nullptr);
	}

	bool TypedBuffer::ValidateParameters(DXGI_FORMAT format, uint32 capacity)
	{
		const uint32 elem_stride = GetDXGIFormatByteStride(format);
		if (elem_stride == 0)
		{
			ERR_MSG("지원되지 않는 DXGI_FORMAT입니다.");
			return false;
		}
		if (capacity == 0)
		{
			ERR_MSG("capacity는 1 이상이어야 합니다.");
			return false;
		}
		if ((uint64)elem_stride * (uint64)capacity > (uint64)(std::numeric_limits<uint32>::max()))
		{
			ERR_MSG("버퍼 크기가 UINT32 범위를 초과합니다.");
			return false;
		}
		return true;
	}

	bool TypedBuffer::CreateBufferImpl(DXGI_FORMAT format, uint32 capacity, D3D11_USAGE buffer_usage, UINT bind_flags, UINT cpu_access_flags, const void* initial_data)
	{
		if (false == ValidateParameters(format, capacity)) { return false; }

		const uint32 stride = GetDXGIFormatByteStride(format);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)(stride * capacity);
		desc.Usage = buffer_usage;
		desc.BindFlags = bind_flags;	
		desc.CPUAccessFlags = cpu_access_flags;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;   // typed buffer는 항상 0

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = initial_data;

		auto* device = GraphicsDevice::GetInst().GetDevice();
		ComPtr<ID3D11Buffer> buffer;
		HRESULT hr = device->CreateBuffer(&desc, initial_data ? &sd : nullptr, buffer.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		ComPtr<ID3D11ShaderResourceView> srv;
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			srv = CreateSRV(buffer.Get(), format, capacity);
			if (!srv)
			{
				ERR_MSG("SRV 생성 실패");
				return false;
			}
		}

		ComPtr<ID3D11UnorderedAccessView> uav;
		if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			uav = CreateUAV(buffer.Get(), format, capacity);
			if (!uav)
			{
				ERR_MSG("UAV 생성 실패");
				return false;
			}
		}

		buffer_ = buffer;
		SRV_ = srv;
		UAV_ = uav;
		buffer_usage_ = buffer_usage;
		format_ = format;
		stride_ = stride;
		capacity_ = capacity;
		count_ = initial_data ? capacity : 0;

		return true;
	}

	ComPtr<ID3D11ShaderResourceView> TypedBuffer::CreateSRV(ID3D11Buffer* buffer, DXGI_FORMAT format, uint32 capacity, uint32 start, uint32 count)
	{
		if (buffer == nullptr)
		{
			ERR_MSG("SRV 생성 실패: buffer가 nullptr입니다.");
			return nullptr;
		}

		// Count == 0 -> 전체 범위
		// 여기서 count를 전체 범위로 변경
		if (count == 0)
		{
			count = (start < capacity) ? (capacity - start) : 0;
		}

		if (count == 0 || start >= capacity || count > capacity - start)
		{
			ERR_MSG("SRV 범위가 버퍼 용량을 초과했습니다.");
			return nullptr;
		}
		ComPtr<ID3D11ShaderResourceView> srv;

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv_desc.Buffer.FirstElement = (UINT)start;
		srv_desc.Buffer.NumElements = (UINT)count;

		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateShaderResourceView(buffer, &srv_desc, srv.GetAddressOf());

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}

		return srv;
	}
	ComPtr<ID3D11UnorderedAccessView> TypedBuffer::CreateUAV(ID3D11Buffer* buffer, DXGI_FORMAT format, uint32 capacity, uint32 start, uint32 count)
	{
		if (buffer == nullptr)
		{
			ERR_MSG("UAV 생성 실패: buffer가 nullptr입니다.");
			return nullptr;
		}

		// Count == 0 -> 전체 범위
		// 여기서 count를 전체 범위로 변경
		if (count == 0)
		{
			count = (start < capacity) ? (capacity - start) : 0;
		}

		//여전히 0이면 문제 있는거
		if (count == 0 || start >= capacity || count > capacity - start)
		{
			ERR_MSG("UAV 범위가 버퍼 용량을 초과했습니다.");
			return nullptr;
		}

		ComPtr<ID3D11UnorderedAccessView> uav;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		uav_desc.Format = format;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav_desc.Buffer.FirstElement = (UINT)start;
		uav_desc.Buffer.NumElements = (UINT)count;
		uav_desc.Buffer.Flags = 0; // APPEND / COUNTER 금지

		auto* device = GraphicsDevice::GetInst().GetDevice();
		HRESULT hr = device->CreateUnorderedAccessView(buffer, &uav_desc, uav.GetAddressOf());
		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return nullptr;
		}

		return uav;
	}
}