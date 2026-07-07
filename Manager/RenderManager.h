#pragma once

#include <Engine/Resource/Graphics/RenderPass/ForwardOpaqueRenderPass.h>

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>
#include <Engine/Core/Enum.h>

struct ID3D11SamplerState;

namespace engine
{
    class RenderPass;
    class ConstantBuffer;
    class Camera;
    class Scene;
    class Renderer;

    class RenderManager
    {
        friend class GameEngine;

        DECLARE_SINGLETON(RenderManager)
        void Init();

    public:
        void Render();
        void FrameEnd();

        void SetMainCamera(s_ptr<Camera> cam) {
            main_cam_ = cam;
        }
        s_ptr<Camera> GetMainCamera() const { return main_cam_.lock(); }

        void OnResolutionChange(uint32 width, uint32 height);
        void OnClearContextStates();

		ForwardOpaqueRenderPass* GetOpaquePass() { return &forward_opaque_pass_; }

    private:
        void CreateSamplerStates();
        void BindPSSamplerStates();

        s_ptr<ConstantBuffer> cb_per_pass_ = {};

        w_ptr<Camera> main_cam_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};

        //Render Pass
        ForwardOpaqueRenderPass forward_opaque_pass_ = {};
    };
}