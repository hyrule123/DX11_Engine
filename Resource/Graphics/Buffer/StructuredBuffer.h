#pragma once
#include <Engine/Resource/Resource.h>

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
    class MapScope
    {
    public:
		MapScope() = default;
        MapScope(ID3D11DeviceContext* ctx, ID3D11Buffer* buf, D3D11_MAP type);
        ~MapScope();

        MapScope(const MapScope&) = delete;
        MapScope& operator=(const MapScope&) = delete;

        bool IsValid() const { return mapped_ok_; }
        void* Data() const { return mapped_.pData; }

    private:
        ID3D11DeviceContext* context_ = nullptr;
        ID3D11Buffer* buffer_ = nullptr;
        D3D11_MAPPED_SUBRESOURCE mapped_ = {};
        bool mapped_ok_ = false;
    };

    class StructuredBuffer :
        public Resource
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
        //TODO: 이거 너무 위험함. 주소를 받아서 작성하는 방식으로 변경할것
        MapScope MapDynamic(ID3D11DeviceContext* context);
        void UnMap(ID3D11DeviceContext* context);

        void BindSRV(ID3D11DeviceContext* context, uint32 slot, ShaderStage::Flags stage_flag);
		void BindUAV(ID3D11DeviceContext* context, uint32 slot);

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

        ComPtr<ID3D11Buffer>              buffer_ = {};
        ComPtr<ID3D11ShaderResourceView>  SRV_ = {};
        ComPtr<ID3D11UnorderedAccessView> UAV_ = {};

        //나중에 Staging Buffer 만드는 코드 필요할듯

        D3D11_USAGE buffer_usage_ = {};
        uint32 stride_ = {};
        uint32 count_ = {};
        uint32 capacity_ = {};
    };
}


