#pragma once
#include <Engine/Resource/GPU/Buffer/Texture2D.h>

#include <Engine/Core/CoreMinimal.h>

#include <span>

struct ID3D11Device;

namespace engine
{
    class Texture2DArray :
        public Texture2D
    {
        ENTITY_INFO(Texture2DArray, Texture2D)
    public:
        Texture2DArray();
        virtual ~Texture2DArray() override;

        // 슬라이스가 slice_pitch 간격으로 연속 배치된 CPU 버퍼로부터 이뮤터블 배열 텍스처 생성
        bool CreateImmutableFromMemory(
            uint32_2 tex_size, uint32_t tex_count,
            DXGI_FORMAT format, std::span<const uint8_t> data, uint32_t row_pitch, uint32_t slice_pitch); // slice_pitch = elem_stride

        //LoadFromFile을 통해 Atlas 텍스처를 로드한 후 호출하면 됨
        bool Slice(uint32_t row_count, uint32_t col_count);

        uint32_t GetFrameCount() const { return frame_count_; }
        uint32_t GetRowCount() const { return row_count_; }
        uint32_t GetColCount() const { return col_count_; }

    private:
        uint32_t frame_count_ = {};
        uint32_t row_count_ = {};
        uint32_t col_count_ = {};
    };
}


