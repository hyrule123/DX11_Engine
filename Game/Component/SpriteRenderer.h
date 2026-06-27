#pragma once
#include <Engine/Game/Component/Renderer.h>

#include <Engine/Core/CoreMinimal.h>

namespace engine
{
    class SpriteRenderer :
        public Renderer
    {
        CLASS_INFO(SpriteRenderer, Renderer)
    public:
        SpriteRenderer();
        virtual ~SpriteRenderer() override;

        virtual void Init() override;
    };

}


