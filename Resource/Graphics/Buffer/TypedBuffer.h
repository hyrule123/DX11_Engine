#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/SmartPointer.h>

#include <Engine/Core/Enum.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/DX11.h>

#include <span>

// DXGI_FORMAT에 있는 타입들을 사용 가능(단일 타입만 가능)
// uint8, uint16, uint32, float4, ...


//
//   - UAV는 kDefault에서만 가능. DYNAMIC/IMMUTABLE에 UAV는 생성 실패.
//   - kDynamic은 CPU_ACCESS_WRITE가 없으면 생성 실패. 끌 수 없다.
//   - kStaging은 BindFlags가 0이어야 한다. 파이프라인에 붙지 않는다.
//   - SRV와 UAV는 동시 바인딩 불가. UAV를 걸면 D3D가 SRV를 언바인드한다.
//     컴퓨트 -> 픽셀 순서로 쓰려면 사이에 null UAV를 바인딩해 끊을 것.

namespace engine
{
    class TypedBuffer :
        public Resource
    {
		ENTITY_INFO(TypedBuffer, Resource)
    public:
		TypedBuffer();
		virtual ~TypedBuffer() override;

		// 로드 후 변하지 않는 데이터용.
		// IMMUTABLE은 초기 데이터가 필수 — data가 null이면 생성이 실패
		bool CreateImmutableBuffer(DXGI_FORMAT format, const void* data, uint32 count);

		// CPU가 매 프레임 갱신하는 데이터용.
		// WRITE_DISCARD가 어차피 전체를 버리므로
		bool CreateDynamicBuffer(DXGI_FORMAT format, uint32 capacity);

		// 컴퓨트 셰이더 입출력용. UAV를 가질 수 있는 유일한 usage.
		// need_uav는 필요할 때만 true — 포맷에 따라 UAV 미지원이면 생성 실패.
		bool CreateDefaultBuffer(DXGI_FORMAT format, uint32 capacity, bool need_uav);

		// CPU -> 버퍼 데이터 업로드.
		// elem_stride는 stride_와 일치해야 하고, elem_count는 capacity_ 이하여야 한다.
		// kImmutable / kNone은 거부.
		// kDefault : 전체 갱신만 지원.
		// kDynamic : WRITE_DISCARD이므로 elem_count 뒤쪽은 정의되지 않은 값이 된다.
		bool Upload(ID3D11DeviceContext* context, const void* data, uint32 elem_stride, uint32 elem_count);
		template <typename T>
		bool Upload(ID3D11DeviceContext* context, std::span<T> data) {
			return Upload(context, data.data(), (uint32)sizeof(T), (uint32)(data.size()));
		}

		// 버퍼를 만든 이후에 쓰세요
		bool Reserve(uint32 new_capacity);

		// 미구현
		bool ReservePreserving(uint32 new_capacity) { return false; };

		uint32 GetCapacity() const { return capacity_; }

		void BindSRV(ID3D11DeviceContext* context, uint32 slot, ShaderStage::Flags stage_flag);
		void BindUAV(ID3D11DeviceContext* context, uint32 slot);

    private:
		bool ValidateParameters(DXGI_FORMAT format, uint32 capacity);

		bool CreateBufferImpl(DXGI_FORMAT format, uint32 capacity, D3D11_USAGE buffer_usage, UINT bind_flags, UINT cpu_access_flags, const void* initial_data);

		// count == 0이면 전체 범위. start + count <= capacity
		ComPtr<ID3D11ShaderResourceView> CreateSRV(ID3D11Buffer* buffer, uint32 capacity , uint32 start = 0, uint32 count = 0);
		ComPtr<ID3D11UnorderedAccessView> CreateUAV(ID3D11Buffer* buffer, uint32 capacity, uint32 start = 0, uint32 count = 0);

        ComPtr<ID3D11Buffer> buffer_ = {};
		ComPtr<ID3D11ShaderResourceView> SRV_ = {};
		ComPtr<ID3D11UnorderedAccessView> UAV_ = {};

		D3D11_USAGE buffer_usage_ = {};
		DXGI_FORMAT format_ = {};
		uint32 stride_ = {};
		uint32 count_ = {};
		uint32 capacity_ = {};
    };
}

