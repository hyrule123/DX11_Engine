#pragma once

#include <Engine/Resource/GPU/RenderPass/ForwardOpaqueRenderPass.h>
#include <Engine/Resource/GPU/RenderPass/PresentPass.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>
#include <Engine/Core/Enum.h>

#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

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

		void SetMainCamera(wh_ptr<Camera> cam) { main_cam_ = cam; }
        wh_ptr<Camera> GetMainCamera() const { return main_cam_; }

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

    private:
        void DebugDraw(ID3D11DeviceContext* context);

        void CreateSamplerStates(ID3D11DeviceContext* context);
        void BindPSSamplerStates(ID3D11DeviceContext* context);
        void CreateDebugRenderObjects(ID3D11DeviceContext* context);

        

        s_ptr<ConstantBuffer> cb_per_pass_camera_ = {};

		wh_ptr<Camera> main_cam_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};

        //Render Pass
        ForwardOpaqueRenderPass forward_opaque_pass_ = {};
		PresentPass present_pass_ = {};

		uint32 resolution_width_ = {};
		uint32 resolution_height_ = {};

		u_ptr<Mesh> debug_rect_mesh_ = {};
		u_ptr<Mesh> debug_circle_mesh_ = {};
        std::vector<DebugDrawPerInstanceData> debug_rect_data_;
		std::vector<DebugDrawPerInstanceData> debug_circle_data_;
		u_ptr<StructuredBuffer> debug_buffer_ = {};
        u_ptr<PipelineState> debug_shader_set_ = {};
    };
}