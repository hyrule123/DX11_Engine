#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Core/CoreMinimal.h>

namespace engine
{
    class SpriteAnimClip :
        public Entity
    {
        CLASS_INFO(SpriteAnimClip, Entity)
    public:
        struct Frame
        {
            uint32 index = {};
            float duration = {};
        };

        SpriteAnimClip();
        virtual ~SpriteAnimClip();

        void AddFrames(const std::vector<uint32>& frame_indices, float total_duration);

        //웬만하면 move semantic
        void AddFrames(std::vector<Frame>&& frames);

        //LValue로 들어오면 어쩔수없이 복사
        void AddFrames(const std::vector<Frame>& frames) {
            AddFrames(std::vector<Frame>(frames));
        }

        const std::vector<Frame>& GetFrames() const { return frames_; }

        void SetLoop(bool is_loop) { is_loop_ = is_loop; }
        
        float GetTotalDuration() const { return total_duration_; }
        bool IsLoop() const { return is_loop_; }

        //Texture Array의 최대 인덱스 벗어나는지 확인용
        uint32 GetMaxFrameIndex() const { return max_frame_idx_; }

        bool IsReady() const { return !(frames_.empty()) || (total_duration_ <= 0.0f); }


    private:
        std::vector<Frame> frames_ = {};
        float total_duration_ = {};
        bool is_loop_ = {};
        uint32 max_frame_idx_ = {};
    };
}


