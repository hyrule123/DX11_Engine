#include "Engine/Core/pch.h"
#include "StructuredBuffer.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/DX11.h>

namespace engine
{
    // 상수 버퍼 규칙
    constexpr bool IsValidStride(uint32 stride)
    {
        return (stride > 0 && (stride % 4) == 0 && stride <= 2048);
    }

	StructuredBuffer::StructuredBuffer()
		: Super(StructuredBuffer::kClassConcreteName)
	{}
	StructuredBuffer::~StructuredBuffer()
	{}
    bool StructuredBuffer::CreateImmutableBuffer(uint32 stride, const void* data, uint32 count)
    {
		return CreateBufferImpl(stride, count, D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE, 0, data);
    }
    bool StructuredBuffer::CreateDynamicBuffer(uint32 stride, uint32 capacity)
    {
        return CreateBufferImpl(stride, capacity, D3D11_USAGE_DYNAMIC,
			D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, nullptr);
    }
    bool StructuredBuffer::CreateDefaultBuffer(uint32 stride, uint32 capacity, bool need_uav)
    {
		return CreateBufferImpl(stride, capacity, D3D11_USAGE_DEFAULT, 
			D3D11_BIND_SHADER_RESOURCE | (need_uav ? D3D11_BIND_UNORDERED_ACCESS : 0u),
			0, nullptr);
    }

    bool StructuredBuffer::Reserve(uint32 new_capacity)
    {
        if (buffer_ == nullptr)
        {
            ERROR_MESSAGE("버퍼가 생성되지 않았습니다.");
            return false;
        }
        if (buffer_usage_ == D3D11_USAGE_IMMUTABLE)
        {
            ERROR_MESSAGE("Immutable 버퍼는 재할당할 수 없습니다.");
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
            HRESULT_ERROR_MESSAGE(hr);
            return false;
        }

        ComPtr<ID3D11ShaderResourceView> srv;
        ComPtr<ID3D11UnorderedAccessView> uav;

        // SRV와 UAV가 생성되어 있을 경우 재생성
        if (SRV_)
        {
            srv = CreateSRVImpl(buffer.Get(), new_capacity);
            if (!srv) { return false; }
        }
        if (UAV_)
        {
            uav = CreateUAVImpl(buffer.Get(), new_capacity);
            if (!uav) { return false; }
        }

        buffer_ = buffer;
        SRV_ = srv;
        UAV_ = uav;
        capacity_ = new_capacity;
        count_ = 0;
        return true;
    }

    bool StructuredBuffer::CreateBufferImpl(uint32 stride, uint32 capacity, D3D11_USAGE buffer_usage, UINT bind_flags, UINT cpu_access, const void* init_data)
    {
        if (!IsValidStride(stride) || capacity == 0) 
        { 
			ERROR_MESSAGE("StructuredBuffer 생성 실패: stride가 4의 배수가 아니거나 0이거나, capacity가 0입니다.");
            return false; 
        }
        if (buffer_usage == D3D11_USAGE_IMMUTABLE && init_data == nullptr)
        {
            ERROR_MESSAGE("data가 nullptr입니다. IMMUTABLE은 초기 데이터가 필수입니다.");
            return false;
        }
		uint64 total_size = (uint64)(stride) * (uint64)(capacity);
		if (total_size > std::numeric_limits<uint32>::max())
		{
			ERROR_MESSAGE("StructuredBuffer 생성 실패: 버퍼 크기가 UINT32 범위를 초과합니다.");
			return false;
		}

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = (UINT)total_size;
        desc.Usage = buffer_usage;
        desc.BindFlags = bind_flags;
        desc.CPUAccessFlags = cpu_access;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = stride;

        D3D11_SUBRESOURCE_DATA sub = {};
        sub.pSysMem = init_data;

        auto* device = GraphicsDevice::GetInst().GetDevice();

        ComPtr<ID3D11Buffer> buffer;
        HRESULT hr = device->CreateBuffer(&desc,
            init_data ? &sub : nullptr,
            buffer.GetAddressOf());
        if (FAILED(hr)) 
        { 
			HRESULT_ERROR_MESSAGE(hr);
            return false; 
        }

		ComPtr<ID3D11ShaderResourceView> srv;
		ComPtr<ID3D11UnorderedAccessView> uav;

		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			srv = CreateSRVImpl(buffer.Get(), capacity);
			if (!srv) { return false; }
		}
		if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			uav = CreateUAVImpl(buffer.Get(), capacity);
			if (!uav) { return false; }
		}

        buffer_ = buffer;
        SRV_ = srv;
        UAV_ = uav;
        stride_ = stride;
        count_ = init_data ? capacity : 0;
        capacity_ = capacity;
        buffer_usage_ = buffer_usage;

        return true;
    }

    ComPtr<ID3D11ShaderResourceView> StructuredBuffer::CreateSRVImpl(ID3D11Buffer* buffer, uint32 capacity, uint32 start, uint32 count)
    {
        if (buffer == nullptr)
        {
            ERROR_MESSAGE("SRV 생성 실패: buffer가 nullptr입니다.");
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
            ERROR_MESSAGE("SRV 범위가 버퍼 용량을 초과했습니다.");
            return nullptr;
        }
        ComPtr<ID3D11ShaderResourceView> srv;

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = (UINT)start;
        srv_desc.Buffer.NumElements = (UINT)count;

        auto* device = GraphicsDevice::GetInst().GetDevice();
        HRESULT hr = device->CreateShaderResourceView(buffer, &srv_desc, srv.GetAddressOf());

        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            return nullptr;
        }

        return srv;
    }
    ComPtr<ID3D11UnorderedAccessView> StructuredBuffer::CreateUAVImpl(ID3D11Buffer* buffer, uint32 capacity, uint32 start, uint32 count)
    {
        if (buffer == nullptr)
        {
            ERROR_MESSAGE("UAV 생성 실패: buffer가 nullptr입니다.");
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
            ERROR_MESSAGE("UAV 범위가 버퍼 용량을 초과했습니다.");
            return nullptr;
        }

        ComPtr<ID3D11UnorderedAccessView> uav;

        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = (UINT)start;
        uav_desc.Buffer.NumElements = (UINT)count;
        uav_desc.Buffer.Flags = 0; // APPEND / COUNTER 금지

        auto* device = GraphicsDevice::GetInst().GetDevice();
        HRESULT hr = device->CreateUnorderedAccessView(buffer, &uav_desc, uav.GetAddressOf());
        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            return nullptr;
        }

        return uav;
    }

    bool StructuredBuffer::Upload(ID3D11DeviceContext* context, void* data, uint32 elem_stride, uint32 elem_count)
	{
        if (data == nullptr)
        {
            ERROR_MESSAGE("data가 nullptr입니다.");
            return false;
        }
        if (elem_stride != stride_)
        {
            ERROR_MESSAGE("stride가 버퍼 포맷과 불일치합니다.");
            return false;
        }
        if (elem_count > capacity_)
        {
            ERROR_MESSAGE("capacity를 초과했습니다.");
            return false;
        }

        switch (buffer_usage_)
        {
        case D3D11_USAGE_IMMUTABLE:
        {
            ERROR_MESSAGE("Immutable 버퍼는 업로드할 수 없습니다.");
            return false;
        }
        case D3D11_USAGE_DEFAULT:
        {
            //참고: UpdateSubresource는 전체 갱신만 지원. 
            //Box의는 16 byte 단위로 정렬되어야 하는 제약이 있음. 따라서 Default 버퍼는 전체 갱신만 지원하도록 제한.
            if (elem_count != capacity_)
            {
                ERROR_MESSAGE("Default 버퍼는 전체 갱신만 지원합니다.");
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
                ERROR_MESSAGE("Map 실패");
                return false;
            }
            std::memcpy(mapped.pData, data, elem_stride * elem_count);
            context->Unmap(buffer_.Get(), 0);
            break;
        }
        case D3D11_USAGE_STAGING:
        {
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            if (FAILED(context->Map(buffer_.Get(), 0, D3D11_MAP_WRITE, 0, &mapped)))
            {
                ERROR_MESSAGE("Staging Map 실패 (cpu_write로 생성했는지 확인)");
                return false;
            }
            std::memcpy(mapped.pData, data, elem_stride * elem_count);
            context->Unmap(buffer_.Get(), 0);
            break;
        }
        default:
            ERROR_MESSAGE("지원되지 않는 BufferUsage입니다.");
            return false;
        }

        count_ = elem_count;

        return true;
	}

    MapScopeDynamic StructuredBuffer::MapDynamic(ID3D11DeviceContext* context)
    {
        if((buffer_usage_ & D3D11_USAGE_DYNAMIC) == 0)
        {
            ERROR_MESSAGE("MapDynamic() can only be used with dynamic buffers.");
            return MapScopeDynamic();
        }
		count_ = 0; // 맵핑 시 기존 데이터는 모두 날아감
        return MapScopeDynamic(context, buffer_.Get(), stride_, capacity_);
    }
    void StructuredBuffer::UnMap(ID3D11DeviceContext* context)
    {
        context->Unmap(buffer_.Get(), 0);
    }
    void StructuredBuffer::BindSRV(ID3D11DeviceContext* context, ShaderStage::Flags stage_buffer_usage, RegisterT slot)
    {
        ASSERT(nullptr != SRV_);
		if (ShaderStage::HasFlag(stage_buffer_usage, ShaderStage::Flags::Vertex))
		{
			context->VSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
		if (ShaderStage::HasFlag(stage_buffer_usage, ShaderStage::Flags::Pixel))
		{
			context->PSSetShaderResources(slot.Get(), 1, SRV_.GetAddressOf());
		}
    }
    void StructuredBuffer::BindUAV(ID3D11DeviceContext* context, RegisterU slot)
    {
        ASSERT(nullptr != UAV_);
        context->CSSetUnorderedAccessViews(slot.Get(), 1, UAV_.GetAddressOf(), nullptr);
    }
    MapScopeDynamic::MapScopeDynamic(ID3D11DeviceContext* ctx, ID3D11Buffer* buf, uint32 stride, uint32 capacity)
		: context_(ctx), buffer_(buf), stride_(stride), capacity_(capacity)
    {
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        HRESULT hr = context_->Map(buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            mapped_ok_ = false;
        }
        else
        {
            p_data_ = static_cast<uint8*>(mapped.pData);
            mapped_ok_ = true;
        }
    }

    MapScopeDynamic::~MapScopeDynamic()
    {
        if (mapped_ok_) { context_->Unmap(buffer_, 0); }
    }
}