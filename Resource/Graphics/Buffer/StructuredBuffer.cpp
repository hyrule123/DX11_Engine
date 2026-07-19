#include "Engine/Core/pch.h"
#include "StructuredBuffer.h"

#include <Engine/Core/DX11.h>


namespace engine
{
	StructuredBuffer::StructuredBuffer()
		: Super(StructuredBuffer::kClassConcreteName)
	{}
	StructuredBuffer::~StructuredBuffer()
	{}
	bool StructuredBuffer::Create(ID3D11Device * device, BufferFlag flag, size_t elem_stride, size_t elem_count, void* initial_data)
	{
        // 1. 방어 코드: 크기가 0이거나 비정상적인 요청 차단
        if (elem_stride == 0 || elem_count == 0)
        {
            ERROR_MESSAGE("데이터 포맷이 비정상입니다.");
            return false;
        }

        ComPtr<ID3D11Buffer>              buffer = {};
        ComPtr<ID3D11ShaderResourceView>  SRV = {};
        ComPtr<ID3D11UnorderedAccessView> UAV = {};

        // 플래그 확인
        bool is_srv = (flag & kSRV) != 0;
        bool is_uav = (flag & kUAV) != 0;
        bool is_dynamic = (flag & kCPUDynamic) != 0;

        //하드웨어 제약 방어: DYNAMIC과 UAV는 동시에 켤 수 없음
        if (is_dynamic && is_uav)
        {
            ERROR_MESSAGE("DYNAMIC과 UAV는 동시에 켤 수 없습니다.");
            return false;
        }

        // 버퍼 기본 설정
        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = (UINT)(elem_stride * elem_count);
        buffer_desc.StructureByteStride = (UINT)elem_stride;
        buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        // BindFlags 설정
        buffer_desc.BindFlags = 0;
        if (is_srv)
        {
            buffer_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        }
        if (is_uav)
        {
            buffer_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }

        // CPU 접근 권한 및 Usage 설정
        if (is_dynamic)
        {
            buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
            buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        }
        else
        {
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.CPUAccessFlags = 0;
        }

        // 초기 데이터(initial_data) 세팅
        D3D11_SUBRESOURCE_DATA sub_data = {};
        D3D11_SUBRESOURCE_DATA* p_sub_data = nullptr;
        if (initial_data != nullptr)
        {
            sub_data.pSysMem = initial_data;
            p_sub_data = &sub_data;
        }

        HRESULT hr = device->CreateBuffer(&buffer_desc, p_sub_data, buffer.GetAddressOf());
        if (FAILED(hr)) 
        { 
            HRESULT_ERROR_MESSAGE(hr);
            return false;
        }

        // SRV 생성
        if (is_srv)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
            srv_desc.Format = DXGI_FORMAT_UNKNOWN; // Structured Buffer 고정
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srv_desc.Buffer.FirstElement = 0;
            srv_desc.Buffer.NumElements = (UINT)elem_count;

            hr = device->CreateShaderResourceView(buffer.Get(), &srv_desc, SRV.GetAddressOf());
            if (FAILED(hr))
            {
                HRESULT_ERROR_MESSAGE(hr);
                return false;
            }
        }

        // UAV 생성
        if (is_uav)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
            uav_desc.Format = DXGI_FORMAT_UNKNOWN; // Structured Buffer 고정
            uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uav_desc.Buffer.FirstElement = 0;
            uav_desc.Buffer.NumElements = (UINT)elem_count;
            uav_desc.Buffer.Flags = 0;

            hr = device->CreateUnorderedAccessView(buffer.Get(), &uav_desc, UAV.GetAddressOf());
            if (FAILED(hr))
            {
                HRESULT_ERROR_MESSAGE(hr);
                return false;
            }
        }

		buffer_ = std::move(buffer);
		SRV_ = std::move(SRV);
		UAV_ = std::move(UAV);
        buffer_flag_ = flag;
		elem_count_ = elem_count;
		elem_stride_ = elem_stride;
		total_byte_size_ = elem_stride * elem_count;

        return true;
	}

	bool StructuredBuffer::Resize(ID3D11Device* device, ID3D11DeviceContext* context, size_t new_count, bool preserve_data)
	{
        // 1. 방어 코드: 크기가 같거나 0이면 무시 (혹은 0일 때의 Clear 로직 필요 시 추가)
        if (!buffer_ || new_count == 0)
        {
            ERROR_MESSAGE("버퍼가 생성되지 않았거나 새로운 크기가 0입니다.");
            return true;
        }

        // 2. 버퍼 백업
        ComPtr<ID3D11Buffer>              old_buffer = buffer_;
        ComPtr<ID3D11ShaderResourceView>  old_SRV = SRV_;
        ComPtr<ID3D11UnorderedAccessView> old_UAV = UAV_;
        BufferFlag old_buffer_flag_ = buffer_flag_;
        size_t old_elem_stride = elem_stride_;
        size_t old_elem_count = elem_count_;
		size_t old_total_byte_size = total_byte_size_;


        // 3. 기존 세팅(flag, stride)을 그대로 사용하여 새로운 크기의 버퍼 생성
        if (!Create(device, buffer_flag_, elem_stride_, new_count, nullptr))
        {
            //실패 시 원복
            buffer_ = old_buffer;
            SRV_ = old_SRV;
            UAV_ = old_UAV;
            buffer_flag_ = old_buffer_flag_;
            elem_stride_ = old_elem_stride;
            elem_count_ = old_elem_count;
            total_byte_size_ = old_total_byte_size;
            return false;
        }

        bool is_dynamic = (buffer_flag_ & kCPUDynamic) != 0;

		ASSERT_MESSAGE(!(is_dynamic && preserve_data), "Dynamic 버퍼는 데이터를 보존하지 않으므로 preserve_data를 true로 설정할 수 없습니다.");

        //Dynamic 모드가 아닐 경우 데이터 복사
        if (!is_dynamic && preserve_data)
        {
            // 혹시나 사이즈를 '줄일 수도' 있으므로, min을 사용
            size_t copy_count = std::min(elem_count_, new_count);

            D3D11_BOX src_box = {};
            src_box.left = 0;
            src_box.right = (UINT)(copy_count * elem_stride_); // 복사할 바이트 크기
            src_box.top = 0;
            src_box.bottom = 1;
            src_box.front = 0;
            src_box.back = 1;

            // 옛날 버퍼(old_buffer)의 src_box 영역만큼 떼어서 새 버퍼(buffer_)의 0번지부터 덮어씀
            context->CopySubresourceRegion(buffer_.Get(), 0, 0, 0, 0, old_buffer.Get(), 0, &src_box);
        }

        //아마도 Create에서 바꿔놨을 테지만... 안전하게
		elem_count_ = (UINT)new_count;
		total_byte_size_ = (UINT)(elem_stride_ * elem_count_);

        return true;
	}

	void StructuredBuffer::Upload(ID3D11DeviceContext* context, void* data, size_t elem_stride, size_t elem_count)
	{
        ASSERT(buffer_);
        ASSERT(data);
        ASSERT(0 < elem_count);
		ASSERT(elem_stride == elem_stride_); // 업로드 데이터의 stride가 버퍼 stride와 일치해야 함
		ASSERT(elem_count <= elem_count_); // 버퍼 사이즈보다 큰 데이터 업로드 시 에러 발생

		size_t byte_size = elem_count * elem_stride_;

        bool is_dynamic = (buffer_flag_ & kCPUDynamic) != 0;

        // --- [케이스 A] DYNAMIC 버퍼 (Map / Unmap) ---
        // CPU 쓰기에 최적화된 영역. 데이터를 통째로 덮어쓰는(DISCARD) 데 가장 빠릅니다.
        if (is_dynamic)
        {
			void* mapped_data = MapForWriteDiscard(context);

            // 확보한 GPU 공유 메모리 주소(pData)에 C++ 데이터를 밀어 넣음
            memcpy(mapped_data, data, (UINT)byte_size);

			UnMap(context);
        }
        // --- [케이스 B] DEFAULT 버퍼 (UpdateSubresource) ---
        // VRAM 전용 영역. CPU가 직접 Map 할 수 없으므로 하드웨어 복사기에게 명령을 내립니다.
        else
        {
            // 업로드할 크기가 전체 버퍼 크기와 완전히 동일하다면 Box 영역 지정(nullptr)을 생략해 최적화할 수 있습니다.
            if (byte_size == total_byte_size_)
            {
                context->UpdateSubresource(buffer_.Get(), 0, nullptr, data, 0, 0);
            }
            else
            {
                // 부분 업데이트일 경우 메모리가 꼬이지 않도록 복사할 범위(Box)를 정확히 명시해야 합니다.
                D3D11_BOX box = {};
                box.left = 0;
                box.right = (UINT)byte_size;
                box.top = 0;
                box.bottom = 1;
                box.front = 0;
                box.back = 1;

                context->UpdateSubresource(buffer_.Get(), 0, &box, data, 0, 0);
            }
        }
	}
    void* StructuredBuffer::MapForWriteDiscard(ID3D11DeviceContext* context)
    {
        ASSERT_MESSAGE(((buffer_flag_ & kCPUDynamic) != 0), "MapForWriteDiscard() can only be used with dynamic buffers.");

        D3D11_MAPPED_SUBRESOURCE mapped_resource = {};

        // 주의: DYNAMIC 구조화 버퍼는 오직 D3D11_MAP_WRITE_DISCARD만 허용됩니다.
        HRESULT hr = context->Map(buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

        if (FAILED(hr))
        {
            HRESULT_ERROR_MESSAGE(hr);
            return nullptr;
        }

        return mapped_resource.pData;
    }
    void StructuredBuffer::UnMap(ID3D11DeviceContext* context)
    {
        context->Unmap(buffer_.Get(), 0);
    }
    void StructuredBuffer::BindSRV(ID3D11DeviceContext* context, uint32 slot, ShaderStage::Flags stage_flag)
    {
        ASSERT(!!SRV_);
		if (stage_flag & ShaderStage::kVS)
		{
			context->VSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
		if (stage_flag & ShaderStage::kPS)
		{
			context->PSSetShaderResources(slot, 1, SRV_.GetAddressOf());
		}
    }
}