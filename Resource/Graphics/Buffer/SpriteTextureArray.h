#pragma once
#include <Engine/Resource/Graphics/Buffer/Texture2D.h>

namespace engine
{


    class SpriteTextureArray :
        public Texture2D
    {
        CLASS_INFO(SpriteTextureArray, Texture2D)
    public:
        SpriteTextureArray();
        virtual ~SpriteTextureArray() override;

        virtual bool LoadFromFile(const stdfs::path& res_path) override;

    private:

    };
}


