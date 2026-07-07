#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
namespace engine
{
    class StructuredBuffer :
        public Resource
    {
        CLASS_INFO(StructuredBuffer, Resource)
    public:
		enum BufferFlagBitMask : uint32
		{
            kSRV = 1 << 0,      // SRV 생성
            kUAV = 1 << 1,      // UAV 생성
			kCPUDynamic = 1 << 2  // 해당 플래그 활성화 시 Dynamic Buffer로 생성
		};
		using BufferFlag = uint32;

        StructuredBuffer();
        virtual ~StructuredBuffer() override;

        bool Create(ID3D11Device* device, BufferFlag flag, size_t elem_stride, size_t elem_count, void* initial_data = nullptr);

        template <typename T>
        bool Create(ID3D11Device* device, BufferFlag flag, size_t elem_count, void* initial_data = nullptr)
        {
            return Create(device, flag, sizeof(T), elem_count, initial_data);
        }

        //Dynamic 버퍼 모드는 데이터를 보존하지 않으므로 새로 업로드할 것
        bool Resize(ID3D11Device* device, ID3D11DeviceContext* context, size_t new_count, bool preserve_data);

        //사이즈 부족 시 에러 발생하므로 확장 필요 여부 확인 필수
        void Upload(ID3D11DeviceContext* context, void* data, size_t byte_size);

		//Dynamic 버퍼 모드에서만 사용 가능. 반드시 UnMap() 호출할것.
		void* MapForWriteDiscard(ID3D11DeviceContext* context);
		void UnMap(ID3D11DeviceContext* context);

		void BindSRV(ID3D11DeviceContext* context, uint32 slot, ShaderStage::Flags stage_flag);

		size_t GetElementStride() const { return elem_stride_; }
		size_t GetElementCount() const { return elem_count_; }
		size_t GetTotalByteSize() const { return total_byte_size_; }

    private:
        ComPtr<ID3D11Buffer>              buffer_ = {};
        ComPtr<ID3D11ShaderResourceView>  SRV_ = {};
        ComPtr<ID3D11UnorderedAccessView> UAV_ = {};

        //나중에 Staging Buffer 만드는 코드 필요할듯

		BufferFlag buffer_flag_ = {};
        size_t elem_stride_ = {};
        size_t elem_count_ = {};
		size_t total_byte_size_ = {};
    };
}


