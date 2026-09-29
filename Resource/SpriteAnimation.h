#pragma once
#include <Engine/Resource/Resource.h>



#include <unordered_set>

namespace engine
{
    class SpriteAnimClip;
    class Texture2DArray;
    class SpriteAnimation :
        public Resource
    {
        ENTITY_INFO(SpriteAnimation, Resource)
    public:
        SpriteAnimation();
        virtual ~SpriteAnimation() override;

        bool IsReady() const { return !!sprite_; }

        std::shared_ptr<Texture2DArray> GetSprite() const { return sprite_; }

        void SetSprite(std::shared_ptr<Texture2DArray> sprite) { sprite_ = std::move(sprite); }

        void AddAnimationClip(const HashedStringView& name, std::unique_ptr<SpriteAnimClip> anim_clip);

        SpriteAnimClip* GetAnimationClip(const HashedStringView& anim_name) const;

        //포인터로 더 빠른 색인을 위함
		bool HasAnimationClip(SpriteAnimClip* clip_ptr) const {
            return (anim_clip_ptrs_.find(clip_ptr) != anim_clip_ptrs_.end());
		}

    private:
        std::shared_ptr<Texture2DArray> sprite_ = {};
        StringHashTable<std::unique_ptr<SpriteAnimClip>> anim_clips_ = {};
		std::unordered_set<SpriteAnimClip*> anim_clip_ptrs_ = {};
    };
}


