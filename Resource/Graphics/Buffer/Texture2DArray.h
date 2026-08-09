#pragma once
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

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

        //LoadFromFile을 통해 Atlas 텍스처를 로드한 후 호출하면 됨
        bool Slice(uint32 row_count, uint32 col_count);

        uint32 GetFrameCount() const { return frame_count_; }
        uint32 GetRowCount() const { return row_count_; }
        uint32 GetColCount() const { return col_count_; }

    private:
        uint32 frame_count_ = {};
        uint32 row_count_ = {};
        uint32 col_count_ = {};
    };
}


