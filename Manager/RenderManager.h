#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>

#include <Engine/Core/Enum.h>

struct ID3D11SamplerState;

namespace engine
{
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

        void AddRenderQueue(s_ptr<Renderer> renderer) { 
            render_queue_.push_back(std::move(renderer)); 
        }

        void OnResolutionChange(uint32 width, uint32 height);
        void OnClearContextStates();

    private:
        void CreateSamplerStates();
        void BindPSSamplerStates();

        s_ptr<ConstantBuffer> cb_per_obj_ = {};
        s_ptr<ConstantBuffer> cb_per_pass_ = {};

        w_ptr<Camera> main_cam_ = {};

        std::vector<s_ptr<Renderer>> render_queue_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};
    };
}