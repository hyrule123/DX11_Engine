#pragma once
#include <Engine/Render/RendererRegistry.h>
#include <Engine/Render/RenderView.h>
#include <Engine/Render/InstanceBufferPool.h>
#include <Engine/Render/RenderTargetPool.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Singleton.h>

#include <Engine/Util/GPUBufferBindingTable.h>

#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

struct ID3D11SamplerState;

namespace engine
{
    class RenderPass;

    class MeshRenderPass;
    class ConstantBuffer;
    class Camera;
    class Scene;
    class Renderer;
    class StructuredBuffer;
    class Mesh;
    class PipelineState;
    class DebugDrawPass;

    struct MeshDrawEntry
    {
        RenderKey key;
        Material* material;
        Mesh* mesh;
        uint32 bounds_id;
        uint32 generation;
    };

    struct ViewEntry
    {
        RenderView          view;
        std::vector<uint32> visible_ids;
        // (나중에) 이 View에서 실행할 패스 목록
    };

    class RenderManager
    {
        friend class GameEngine;

        DECLARE_SINGLETON(RenderManager)
        void Init();

    public:
        void Render();
        void FrameEnd();

        template <typename T>
        void RegisterRenderPass(std::unique_ptr<T> render_pass) {
            static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
            RegisterRenderPassImpl(std::move(render_pass), std::is_base_of_v<MeshRenderPass, T>);
        }

        void SetRenderPassOrder(std::vector<render_pass::ID> order);

        void RegisterRenderer(Renderer* renderer);
        void UnregisterRenderer(Renderer* renderer);

        void MarkBoundsDirty(const Renderer* renderer);

        void SetMainRenderTarget(std::shared_ptr<RenderTargetGroup> rtg) { main_render_target_override_ = std::move(rtg); }

        void SetMainCamera(weak_handle_ptr<Camera> cam) { main_cam_ = cam; }
        weak_handle_ptr<Camera> GetMainCamera() const { return main_cam_; }

        void SetMainViewClearSettings(const ClearSettings& clear_settings) { main_view_clear_setting_ = clear_settings; }

        void OnScreenSizeChange(uint32 width, uint32 height);
        void OnClearContextStates();

        void PushDebugRect(const DebugDrawPerInstanceData& debug_data);
        void PushDebugCircle(const DebugDrawPerInstanceData& debug_data);

        GPUBufferBindingTable& GetPerFrameBindingTable() { return per_frame_binding_table_; }

		RenderTargetPool& GetRenderTargetPool() { return render_target_pool_; }

    private:
        void CreateDebugDrawPass();
        MeshRenderPass* GetMeshRenderPass(render_pass::ID id);
        void CullView(ViewEntry& view_entry);
        void ExpandToMeshPasses(const ViewEntry& view_entry);

        RenderTargetGroup* GetMainRenderTarget() const;

        void RegisterRenderPassImpl(std::unique_ptr<RenderPass> render_pass, bool is_mesh_renderpass);

        void CreateSamplerStates(ID3D11DeviceContext* context);
        void BindPSSamplerStates(ID3D11DeviceContext* context);

    private:
        RenderTargetPool render_target_pool_;
        
        std::shared_ptr<RenderTargetGroup> main_render_target_override_; // 없을 시 SwapChain RTG 사용

        std::shared_ptr<ConstantBuffer> cb_per_pass_camera_;

        weak_handle_ptr<Camera> main_cam_;
        ViewEntry main_view_;
        ClearSettings main_view_clear_setting_;

        std::array<std::unique_ptr<RenderPass>, (size_t)render_pass::ID::kCount> render_passes_;
        render_pass::Flags mesh_pass_mask_;
        render_pass::Flags active_mesh_passes_;
        std::vector<render_pass::ID> render_pass_order_;
		InstanceBufferPool instance_buffer_pool_;
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_;

        RendererRegistry renderer_registry_;

		uint32 resolution_width_;
		uint32 resolution_height_;

        GPUBufferBindingTable per_frame_binding_table_;

		DebugDrawPass* debug_draw_pass_;

        bool is_rendering;
    };
}