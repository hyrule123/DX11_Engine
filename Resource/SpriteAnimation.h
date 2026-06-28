#pragma once
#include <Engine/Resource/Resource.h>

namespace engine
{
    struct AnimationClip
    {
        std::vector<uint32> frames = {};
        float duration = {};
        bool is_loop = {};
    };

    class Texture2DArray;

    class SpriteAnimation :
        public Resource
    {
        CLASS_INFO(SpriteAnimation, Resource)
    public:
        SpriteAnimation();
        virtual ~SpriteAnimation() override;

        bool IsReady() const { return !!sprite_; }

        s_ptr<Texture2DArray> GetSprite() const { return sprite_; }

        void SetSprite(s_ptr<Texture2DArray> sprite) { sprite_ = std::move(sprite); }

        void AddAnimationClip(const std::string_view name, const AnimationClip& anim_clip);

        //Unordered map은 내부 컨테이너의 주소 일관성이 보장된다.(즉 포인터로 반환해도 에러 발생 X)
        const AnimationClip* GetAnimationClip(const std::string_view anim_name) const;

    private:
        s_ptr<Texture2DArray> sprite_ = {};
        StringHashMap<AnimationClip> anim_clips_ = {};
    };
}


