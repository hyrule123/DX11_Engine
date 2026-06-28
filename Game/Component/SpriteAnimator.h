#pragma once
#include <Engine/Game/Component/Component.h>

namespace engine
{
    class SpriteRenderer;
    class SpriteAnimation;
    struct AnimationClip;

    class SpriteAnimator :
        public Component
    {
        CLASS_INFO(SpriteAnimator, Component)
        COMPONENT_CATEGORY(ComponentCategory::kAnimator)
    public:
        SpriteAnimator();
        virtual ~SpriteAnimator() override;

        virtual void Init() override;
        virtual void Awake() override;

        virtual void LateUpdate() override;

        bool SetSpriteAnimation(const stdfs::path& res_path);
        void SetSpriteAnimation(s_ptr<SpriteAnimation> anim) { anim_ = anim; }

        bool Play(const std::string_view anim_name);

    private:
        w_ptr<SpriteRenderer> renderer_ = {};
        s_ptr<SpriteAnimation> anim_ = {};

        const AnimationClip* playing_clip_ = {};

        float acc_deltatime_ = {};
        uint32 cur_frame_idx_ = {};
    };
}


