#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>

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

    private:
        s_ptr<ConstantBuffer> cb_per_obj_ = {};
        s_ptr<ConstantBuffer> cb_per_pass_ = {};

        w_ptr<Camera> main_cam_ = {};

        std::vector<s_ptr<Renderer>> render_queue_ = {};
    };
}


