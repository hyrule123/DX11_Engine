#pragma once
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

struct ID3D11Device;

namespace engine
{


    class SpriteTextureArray :
        public Texture2D
    {
        CLASS_INFO(SpriteTextureArray, Texture2D)
    public:
        SpriteTextureArray();
        virtual ~SpriteTextureArray() override;

        //LoadFromFile을 통해 Atlas 텍스처를 로드한 후 호출하면 됨
        bool CreateSpriteFromAtlas(uint32 row_frames, uint32 column_frames);

    private:
        uint32 frame_count_ = {};
    };
}


