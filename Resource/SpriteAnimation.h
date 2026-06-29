#pragma once
#include <Engine/Resource/Resource.h>


namespace engine
{
    class SpriteAnimClip;
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

        void AddAnimationClip(const std::string_view name, s_ptr<SpriteAnimClip> anim_clip);

        s_ptr<SpriteAnimClip> GetAnimationClip(const std::string_view anim_name) const;

    private:
        s_ptr<Texture2DArray> sprite_ = {};
        StringHashMap<s_ptr<SpriteAnimClip>> anim_clips_ = {};
    };
}


