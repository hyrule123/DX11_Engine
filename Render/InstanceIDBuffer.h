#pragma once

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/DX11.h>

namespace engine
{
	// 슬롯 31(vertex::instance_id::kInputSlot)에 바인딩되는 0, 1, 2, ... 정수열 정점 버퍼.
	// - 평범한 정점 버퍼이며, per-instance로 읽히는 것은 InputLayout desc(PER_INSTANCE_DATA)가 결정한다.
	// - Draw의 StartInstanceLocation만큼 밀려서 읽히므로, VS의 INSTANCE_ID = 인스턴스 버퍼의 전역 인덱스.
	// - IMMUTABLE이라 내용이 바뀌지 않는다. 용량이 부족하면 더 큰 버퍼로 재생성한다.
	// - 범위를 넘어 읽으면 D3D11은 0을 반환하므로(에러 없음) 용량 보장이 필수.
	class InstanceIDBuffer
	{
	public:
		InstanceIDBuffer();
		~InstanceIDBuffer();

		InstanceIDBuffer(const InstanceIDBuffer&) = delete;
		InstanceIDBuffer& operator=(const InstanceIDBuffer&) = delete;

		void Init(ID3D11DeviceContext* context);

		// 최소 count개를 담도록 보장. 재생성되었다면 즉시 다시 바인딩한다.
		bool EnsureCapacity(ID3D11DeviceContext* context, uint32_t count);

		// 프레임 시작 시, 그리고 ClearState 이후에 호출
		void Bind(ID3D11DeviceContext* context) const;

		uint32_t GetCapacity() const { return capacity_; }

	private:
		bool Recreate(uint32_t new_capacity);

		static constexpr uint32_t kMinCapacity = 4096;	// 16KB

		ComPtr<ID3D11Buffer> buffer_ = {};
		uint32_t capacity_ = 0;

		bool has_initialized_ = false;
	};
}