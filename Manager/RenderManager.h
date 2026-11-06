#pragma once
#include <Engine/Render/RendererRegistry.h>
#include <Engine/Render/RenderView.h>
#include <Engine/Render/InstanceBufferPool.h>
#include <Engine/Render/RenderTargetPool.h>
#include <Engine/Render/RenderPass/RenderPassTypes.h>

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
        uint32_t bounds_id;
        uint32_t generation;
    };

    struct ViewEntry
    {
        RenderView          view;
        std::vector<uint32_t> visible_ids;
        // (나중에) 이 View에서 실행할 패스 목록
    };

    class RenderManager
    {
        friend class GameEngine;

        DECLARE_SINGLETON(RenderManager)
        void Init();

    public:
        void Render(const RenderViewDesc& desc);
        void FrameEnd();

        template <typename T>
        void RegisterRenderPass(std::unique_ptr<T> render_pass) {
            static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
            RegisterRenderPassImpl(std::move(render_pass), std::is_base_of_v<MeshRenderPass, T>);
        }

        void RegisterRenderer(Renderer* renderer);
        void UnregisterRenderer(Renderer* renderer);

        void MarkBoundsDirty(const Renderer* renderer);

        void OnScreenSizeChange(uint32_t width, uint32_t height);
        void OnClearContextStates();
        
#if ENABLE_DEBUG_DRAW
        void PushDebugRect(const DebugDrawPerInstanceData& debug_data);
        void PushDebugCircle(const DebugDrawPerInstanceData& debug_data);
#else
        void PushDebugRect(const DebugDrawPerInstanceData&) {};
        void PushDebugCircle(const DebugDrawPerInstanceData&) {};
#endif // ENABLE_DEBUG_DRAW

        GPUBufferBindingTable& GetPerFrameBindingTable() { 
            ASSERT_MAIN_THREAD();
            return per_frame_binding_table_; 
        }

		RenderTargetPool& GetRenderTargetPool() { 
            ASSERT_MAIN_THREAD();
            return render_target_pool_;
        }

        void ReleaseCustomPasses();

        // Renderer Registry는 반드시 비어있어야 함
		void CheckRendererRegistryConsistency();

    private:
        MeshRenderPass* GetMeshRenderPass(render_pass::ID id);
        void CullView(ViewEntry& view_entry);
        void ExpandToMeshPasses(const ViewEntry& view_entry);

        void RegisterRenderPassImpl(std::unique_ptr<RenderPass> render_pass, bool is_mesh_renderpass);

        void CreateSamplerStates(ID3D11DeviceContext* context);
        void BindPSSamplerStates(ID3D11DeviceContext* context);

    private:
        RenderTargetPool render_target_pool_ = {};

        std::shared_ptr<ConstantBuffer> cb_per_pass_camera_ = {};

        ViewEntry main_view_ = {};

        std::array<std::unique_ptr<RenderPass>, (size_t)render_pass::ID::kCount> render_passes_ = {};
        render_pass::Flags mesh_pass_mask_ = {};
        render_pass::Flags active_mesh_passes_ = {};
        InstanceBufferPool instance_buffer_pool_ = {};
        
        //Slot에 꽃아두고 계속 사용
        std::vector<ComPtr<ID3D11SamplerState>> sampler_states_ = {};

        RendererRegistry renderer_registry_ = {};

		uint32_t resolution_width_ = {};
		uint32_t resolution_height_ = {};

        GPUBufferBindingTable per_frame_binding_table_; // 생성자 인자 있음: CPP 초기화

		std::unique_ptr<DebugDrawPass> debug_draw_pass_ = {};

        bool is_rendering = {};
    };
}