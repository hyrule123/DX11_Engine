#pragma once
#include <Engine/Game/Component/Component.h>

namespace engine
{
    class SpriteRenderer;
    class SpriteAnimation;

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

    private:
        w_ptr<SpriteRenderer> renderer_ = {};
        s_ptr<SpriteAnimation> anim_ = {};
    };
}


