#pragma once

#include <Engine/Resource/Graphics/RenderPass/ForwardOpaqueRenderPass.h>
#include <Engine/Resource/Graphics/RenderPass/PresentPass.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>
#include <Engine/Core/Enum.h>

#include <Engine/HLSL/Debug/Debug.hlsli>

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

        void SetMainCamera(s_ptr<Camera> cam);
        s_ptr<Camera> GetMainCamera() const { return main_cam_.lock(); }

        void OnScreenSizeChange(uint32 width, uint32 height);
        void OnClearContextStates();

		ForwardOpaqueRenderPass* GetOpaquePass() { return &forward_opaque_pass_; }
		PresentPass* GetPresentPass() { return &present_pass_; }

        void DrawDebugRect(const DebugInstanceData& debug_data) {
            debug_rect_data_.push_back(debug_data);
        }

    private:
        void CreateSamplerStates(ID3D11Device* device, ID3D11DeviceContext* context);
        void BindPSSamplerStates(ID3D11DeviceContext* context);
        void CreateDebugRenderObjects(ID3D11Device* device, ID3D11DeviceContext* context);

        s_ptr<ConstantBuffer> cb_per_pass_ = {};

        w_ptr<Camera> main_cam_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};

        //Render Pass
        ForwardOpaqueRenderPass forward_opaque_pass_ = {};
		PresentPass present_pass_ = {};

		uint32 resolution_width_ = {};
		uint32 resolution_height_ = {};

		u_ptr<Mesh> debug_rect_mesh_ = {};
        std::vector<DebugInstanceData> debug_rect_data_;
		u_ptr<StructuredBuffer> debug_buffer_ = {};
        u_ptr<GraphicsShaderSet> debug_shader_set_ = {};
    };
}