#pragma once
#include <Engine/Game/Component/Renderer.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/HLSL/Sprite/Sprite.hlsli>

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
		virtual void LateUpdate() override;

        virtual void WritePerObjData(void* ptr) final;

		void SetFrameIndex(uint32 frame_index) { per_obj_data_.sprite_frame_idx = frame_index; }

    private:
        SpriteInstanceData per_obj_data_ = {};
    };
}


