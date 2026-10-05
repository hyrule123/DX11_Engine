#pragma once
#include <Engine/Render/RenderPass/RenderPass.h>

namespace engine
{
    class ForwardBlendPass :
        public RenderPass
    {
    public:
        ForwardBlendPass();
        virtual ~ForwardBlendPass() override;

        struct RenderItem
        {
            uint64 sort_key = {};   // Render Key + 각 Pass별 정책이 반영된 최종 정렬 키
            RenderKey key = {};
            Renderer* renderer = {};

            auto operator<=>(const RenderItem& other) const {
                return sort_key <=> other.sort_key;
            }
            auto operator == (const RenderItem& other) const {
                return sort_key == other.sort_key;
            }
        };

        void SubmitRenderItem(const RenderItem& item) {
            render_queue_.push_back(item);
        }

        virtual void Execute(ID3D11DeviceContext* context, const RenderPassContext& pass_context) final;

    private:
        std::vector<RenderItem> render_queue_ = {};

        std::vector<uint32> culled_renderers_ = {};
    };
}


