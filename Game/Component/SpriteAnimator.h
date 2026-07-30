#pragma once
#include <Engine/Game/Component/Component.h>

namespace engine
{
    class SpriteRenderer;
    class SpriteAnimation;
    class SpriteAnimClip;

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

        bool SetSpriteAnimation(const HashedStringView& res_path);
        void SetSpriteAnimation(s_ptr<SpriteAnimation> anim) { anim_ = anim; }

        bool Play(const HashedStringView& anim_name);
        bool Play(SpriteAnimClip* clip_ptr);

        bool SwitchAnimKeepFrame(const HashedStringView& anim_name);
        bool SwitchAnimKeepFrame(SpriteAnimClip* clip_ptr);

        void AddNotifyCallback(const HashedStringView& notify_name, std::function<void()> callback) {
            if(!notify_name.IsEmpty() && callback) {
                frame_notify_callbacks_[notify_name].push_back(callback);
            }
        }
    private:
		//참고: Internal 함수는 Null Check를 하지 않음. 호출 전 반드시 확인 필요
        void PlayInternal(SpriteAnimClip* clip_ptr);
        bool SwitchAnimKeepFrameInternal(SpriteAnimClip* clip_ptr);
		void TriggerNotify(uint32 frame_idx);

        w_ptr<SpriteRenderer> renderer_ = {};
        s_ptr<SpriteAnimation> anim_ = {};

        SpriteAnimClip* playing_clip_ = {};

        bool is_playing_ = {};

        //상태 캐시
        float acc_deltatime_ = {};
        uint32 cur_frame_idx_ = {};
        uint32 clip_frame_total_count_ = {};
        float cur_frame_duration_= {};

		StringHashTable<std::vector<std::function<void()>>> frame_notify_callbacks_ = {};
    };
}


