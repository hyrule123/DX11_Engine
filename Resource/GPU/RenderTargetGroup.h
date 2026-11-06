#pragma once

#include <Engine/Resource/Resource.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Constants.h>
#include <Engine/Core/DX11.h>
#include <Engine/Core/Enum.h>

#include <Engine/Render/RenderPass/RenderPassTypes.h>
#include <Engine/Render/RenderTypes.h>


#include <array>

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DeviceContext;

namespace engine
{
    class RenderTarget;
    class DepthStencilTarget;

    class RenderTargetGroup 
        : public Resource
    {
        ENTITY_INFO(RenderTargetGroup, Resource)
    public:
        using RenderTargetArray = std::array<std::shared_ptr<RenderTarget>, kMaxRenderTargetCount>;

        RenderTargetGroup();
        virtual ~RenderTargetGroup() override;

        void SetRenderTargets(const RenderTargetArray& rtv_arr);
        void SetDepthStencilTarget(std::shared_ptr<DepthStencilTarget> dsv) { dsv_ = std::move(dsv); }

		// start_slot으로부터 8장의 RenderTargetView를 동시에 바인딩함.
		void BindSRVs(ID3D11DeviceContext* context, ShaderStageFlags stage_flags, RegisterT start_slot);
		void UnbindSRVs(ID3D11DeviceContext* context, ShaderStageFlags stage_flags, RegisterT start_slot);

        void BindOutputMerger(ID3D11DeviceContext* context);
		void UnbindOutputMerger(ID3D11DeviceContext* context);

		void Clear(ID3D11DeviceContext* context, const ClearSettings& settings);
        void ClearRenderTargetView(ID3D11DeviceContext* context, std::array<float, 4> clear_color);
        void ClearDepthStencilView(ID3D11DeviceContext* context, float depth, uint8_t stencil);

        void Reset();

        uint32_2 GetTargetSize() const;

    private:
        RenderTargetArray render_target_buffers_ = {};
        std::shared_ptr<DepthStencilTarget> dsv_ = {};

		bool srv_bound_ = false;
    };
}


