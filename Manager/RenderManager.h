#pragma once

#include <Engine/Render/RenderPass/ForwardOpaqueRenderPass.h>
#include <Engine/Render/RenderPass/PresentPass.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>

#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

#include <Engine/Util/GPUBufferBindingTable.h>

struct ID3D11SamplerState;

namespace engine
{
    class RenderPass;
    class ConstantBuffer;
    class Camera;
    class Scene;
    class Renderer;
    class StructuredBuffer;
    class Mesh;

    class RenderManager
    {
        friend class GameEngine;

        DECLARE_SINGLETON(RenderManager)
        void Init();

    public:
        void Render();
        void FrameEnd();

		void RefreshRenderer(Renderer* renderer);
		void UnregisterRenderer(Renderer* renderer);

		void MarkBoundsDirty(const Renderer* renderer);

		void SetMainCamera(weak_handle_ptr<Camera> cam) { main_cam_ = cam; }
        weak_handle_ptr<Camera> GetMainCamera() const { return main_cam_; }

        void OnScreenSizeChange(uint32 width, uint32 height);
        void OnClearContextStates();

		ForwardOpaqueRenderPass* GetOpaquePass() { return &forward_opaque_pass_; }
		PresentPass* GetPresentPass() { return &present_pass_; }

        void DrawDebugRect(const DebugDrawPerInstanceData& debug_data) {
            debug_rect_data_.push_back(debug_data);
        }
		void DrawDebugCircle(const DebugDrawPerInstanceData& debug_data) {
			debug_circle_data_.push_back(debug_data);
		}

		StructuredBuffer* AcquireInstanceBuffer(uint32 byte_stride, uint32 elem_count);

		GPUBufferBindingTable& GetPerFrameBindingTable() { return per_frame_binding_table_; }

    private:
        void DebugDraw(ID3D11DeviceContext* context);

        void CreateSamplerStates(ID3D11DeviceContext* context);
        void BindPSSamplerStates(ID3D11DeviceContext* context);
        void CreateDebugRenderObjects(ID3D11DeviceContext* context);

        std::shared_ptr<ConstantBuffer> cb_per_pass_camera_ = {};

		weak_handle_ptr<Camera> main_cam_ = {};

		// Key: byte stride, Value: StructuredBuffer
		std::unordered_map<uint32, std::unique_ptr<StructuredBuffer>> instance_buffer_per_stride_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};

        //Render Passes
        // 공통 함수 호출을 위한 포인터 저장소
		std::array<RenderPass*, (size_t)RenderPassOrder::kCount> render_passes_ = {};

        ForwardOpaqueRenderPass forward_opaque_pass_ = {};
		PresentPass present_pass_ = {};

		uint32 resolution_width_ = {};
		uint32 resolution_height_ = {};

		std::unique_ptr<Mesh> debug_rect_mesh_ = {};
		std::unique_ptr<Mesh> debug_circle_mesh_ = {};
        std::vector<DebugDrawPerInstanceData> debug_rect_data_;
		std::vector<DebugDrawPerInstanceData> debug_circle_data_;
		std::unique_ptr<StructuredBuffer> debug_buffer_ = {};
        std::unique_ptr<PipelineState> debug_shader_set_ = {};

        GPUBufferBindingTable per_frame_binding_table_;
    };
}