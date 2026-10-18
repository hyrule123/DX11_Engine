#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>
#include <Engine/Resource/GPU/Buffer/MapScopeWriteDiscard.h>

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
    class StructuredBuffer
        : public Resource
		, public ShaderResource
    {
        ENTITY_INFO(StructuredBuffer, Resource);

		friend class MapScopeWriteDiscard;
    public:
        StructuredBuffer();
        virtual ~StructuredBuffer() override;

        bool CreateImmutableBuffer(uint32_t stride, const void* data, uint32_t count);
        template <typename T>
		bool CreateImmutableBuffer(std::span<T> data)
		{
			return CreateImmutableBuffer((uint32_t)sizeof(T), data.data(), (uint32_t)data.size());
		}

        bool CreateDynamicBuffer(uint32_t stride, uint32_t capacity);
		template <typename T>
        bool CreateDynamicBuffer(uint32_t capacity) {
			return CreateDynamicBuffer((uint32_t)sizeof(T), capacity);
        }

        bool CreateDefaultBuffer(uint32_t stride, uint32_t capacity, bool need_uav);
		template <typename T>
        bool CreateDefaultBuffer(uint32_t capacity, bool need_uav) {
            return CreateDefaultBuffer((uint32_t)sizeof(T), capacity, need_uav);
        }

        // No Preserve
        bool Reserve(uint32_t new_capacity);

        template <typename T>
        bool Upload(ID3D11DeviceContext* context, const std::span<T> data)
        {
            return Upload(context, (void*)data.data(), (uint32_t)sizeof(T), (uint32_t)data.size());
        }

        //사이즈 부족 시 에러 발생하므로 확장 필요 여부 확인 필수
        //elem_stride: 사이즈 일치여부 확인 용
        bool Upload(ID3D11DeviceContext* context, void* data, uint32_t elem_stride, uint32_t elem_count);

        //Dynamic 버퍼 모드에서만 사용 가능. 반드시 UnMap() 호출할것.
		//WriteDiscard 모드로 맵핑되므로 기존 데이터는 모두 날아감.
        MapScopeWriteDiscard MapWriteDiscard(ID3D11DeviceContext* context);

        void BindSRV(ID3D11DeviceContext* context, ShaderStageFlags stage_flag, RegisterT slot);
		void BindUAV(ID3D11DeviceContext* context, RegisterU slot);

        uint32_t GetElementStride() const { return stride_; }
        uint32_t GetElementCount() const { return count_; }
        uint32_t GetCapacity() const { return capacity_; }

    private:
        void UnMap(ID3D11DeviceContext* context);
        bool CreateBufferImpl(uint32_t stride, uint32_t capacity,
            D3D11_USAGE buffer_usage,
            UINT bind_flags, UINT cpu_access, const void* init_data);

        // count == 0이면 전체 범위. start + count <= capacity
        ComPtr<ID3D11ShaderResourceView> CreateSRVImpl(ID3D11Buffer* buffer, uint32_t capacity, uint32_t start = 0, uint32_t count = 0);
        ComPtr<ID3D11UnorderedAccessView> CreateUAVImpl(ID3D11Buffer* buffer, uint32_t capacity, uint32_t start = 0, uint32_t count = 0);

        ComPtr<ID3D11UnorderedAccessView> UAV_ = {};
        ComPtr<ID3D11Buffer>              buffer_ = {};

        //나중에 Staging Buffer 만드는 코드 필요할듯

        D3D11_USAGE buffer_usage_ = {};
        uint32_t stride_ = {};
        uint32_t count_ = {};
        uint32_t capacity_ = {};
    };
}


