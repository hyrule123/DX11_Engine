#pragma once
#include <Engine/Render/RenderPass/RenderPass.h>

#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

namespace engine
{
    class Mesh;
    class StructuredBuffer;
    class PipelineState;

    class DebugDrawPass :
        public RenderPass
    {
        ENTITY_INFO(DebugDrawPass, RenderPass);

    public:
        DebugDrawPass();
		virtual ~DebugDrawPass() override;

        virtual void Init() override;

		virtual void Execute(const RenderContext& ctx) final;

		void PushDebugRect(const DebugDrawPerInstanceData& debug_data) {
			debug_rect_data_.push_back(debug_data);
		}
		void PushDebugCircle(const DebugDrawPerInstanceData& debug_data) {
			debug_circle_data_.push_back(debug_data);
		}

    private:
        std::unique_ptr<Mesh> debug_rect_mesh_;
        std::unique_ptr<Mesh> debug_circle_mesh_;
        std::vector<DebugDrawPerInstanceData> debug_rect_data_;
        std::vector<DebugDrawPerInstanceData> debug_circle_data_;
        std::unique_ptr<StructuredBuffer> debug_buffer_;
        std::unique_ptr<PipelineState> debug_shader_set_;
    };
}


