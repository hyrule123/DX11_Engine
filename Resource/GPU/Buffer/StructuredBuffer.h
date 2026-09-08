#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

#include <Engine/Core/Debug.h>

#include <span>

/*
IMMUTABLE   CPU갱신불가	        / SRV O / UAV X
DEFAULT	    UpdateSubresource   / SRV O / UAV O
DYNAMIC	    Map(WRITE_DISCARD)  / SRV O / UAV X
STAGING	    Map(READ / WRITE)   / SRV X / UAV X
*/

struct ID3D11Device;
struct ID3D11DeviceContext;
namespace engine
{
    class MapScopeDynamic
    {
    public:
        MapScopeDynamic() = default;
        MapScopeDynamic(ID3D11DeviceContext* ctx, ID3D11Buffer* buf, uint32 stride, uint32 capacity);
        ~MapScopeDynamic();

        MapScopeDynamic(const MapScopeDynamic&) = delete;
        MapScopeDynamic& operator=(const MapScopeDynamic&) = delete;

        bool IsValid() const { return mapped_ok_; }
        uint32 Count() const { return count_; }
        bool IsFull() const { return count_ >= capacity_; }

        // 슬롯을 하나 소비한다. 실패 시 빈 블록.
        DataBlock Allocate()
        {
            ASSERT(mapped_ok_ && count_ < capacity_);
            if (!mapped_ok_ || count_ >= capacity_) { return {}; }
            return { p_data_ + (size_t)stride_ * count_++, stride_ };
        }

    private:
        ID3D11DeviceContext* const  context_ = nullptr;
        ID3D11Buffer* const buffer_ = nullptr;
        const uint32 stride_ = 0;
        const uint32 capacity_ = 0;

        uint8* p_data_ = nullptr;
        uint32 count_ = 0;
        bool mapped_ok_ = false;
    };

    class StructuredBuffer
        : public Resource
		, public ShaderResource
    {
        ENTITY_INFO(StructuredBuffer, Resource)
    public:
        StructuredBuffer();
        virtual ~StructuredBuffer() override;

        bool CreateImmutableBuffer(uint32 stride, const void* data, uint32 count);
        template <typename T>
		bool CreateImmutableBuffer(std::span<T> data)
		{
			return CreateImmutableBuffer((uint32)sizeof(T), data.data(), (uint32)data.size());
		}

        bool CreateDynamicBuffer(uint32 stride, uint32 capacity);
		template <typename T>
        bool CreateDynamicBuffer(uint32 capacity) {
			return CreateDynamicBuffer((uint32)sizeof(T), capacity);
        }

        bool CreateDefaultBuffer(uint32 stride, uint32 capacity, bool need_uav);
		template <typename T>
        bool CreateDefaultBuffer(uint32 capacity, bool need_uav) {
            return CreateDefaultBuffer((uint32)sizeof(T), capacity, need_uav);
        }

        // No Preserve
        bool Reserve(uint32 new_capacity);

        template <typename T>
        bool Upload(ID3D11DeviceContext* context, const std::span<T> data)
        {
            return Upload(context, (void*)data.data(), (uint32)sizeof(T), (uint32)data.size());
        }

        //사이즈 부족 시 에러 발생하므로 확장 필요 여부 확인 필수
        //elem_stride: 사이즈 일치여부 확인 용
        bool Upload(ID3D11DeviceContext* context, void* data, uint32 elem_stride, uint32 elem_count);

        //Dynamic 버퍼 모드에서만 사용 가능. 반드시 UnMap() 호출할것.
		//WriteDiscard 모드로 맵핑되므로 기존 데이터는 모두 날아감.
        MapScopeDynamic MapDynamic(ID3D11DeviceContext* context);
        void UnMap(ID3D11DeviceContext* context);

        void BindSRV(ID3D11DeviceContext* context, ShaderStage::Flags stage_flag, RegisterT slot);
		void BindUAV(ID3D11DeviceContext* context, RegisterU slot);

        uint32 GetElementStride() const { return stride_; }
        uint32 GetElementCount() const { return count_; }
        uint32 GetCapacity() const { return capacity_; }

    private:
        bool CreateBufferImpl(uint32 stride, uint32 capacity,
            D3D11_USAGE buffer_usage,
            UINT bind_flags, UINT cpu_access, const void* init_data);

        // count == 0이면 전체 범위. start + count <= capacity
        ComPtr<ID3D11ShaderResourceView> CreateSRVImpl(ID3D11Buffer* buffer, uint32 capacity, uint32 start = 0, uint32 count = 0);
        ComPtr<ID3D11UnorderedAccessView> CreateUAVImpl(ID3D11Buffer* buffer, uint32 capacity, uint32 start = 0, uint32 count = 0);

        ComPtr<ID3D11UnorderedAccessView> UAV_ = {};
        ComPtr<ID3D11Buffer>              buffer_ = {};

        //나중에 Staging Buffer 만드는 코드 필요할듯

        D3D11_USAGE buffer_usage_ = {};
        uint32 stride_ = {};
        uint32 count_ = {};
        uint32 capacity_ = {};
    };
}


