#pragma once
#include <Engine/Render/RenderPass/RenderPass.h>

#include <Engine/Render/RenderTypes.h>

namespace engine
{
	class Mesh;
    class PipelineState;
	class RenderTargetGroup;

	class PresentPass
		: public RenderPass
    {
		ENTITY_INFO(PresentPass, RenderPass);
    public:
        PresentPass();
		virtual ~PresentPass() override;

		virtual void Init() override;
		virtual void Execute(const RenderContext& ctx) override;

		void SetClearSettings(const ClearSettings& settings) { clear_settings_ = settings; }

    private:
		std::unique_ptr<Mesh> mesh_ = {};
		std::unique_ptr<PipelineState> shader_set_ = {};

		ClearSettings clear_settings_ = {};
    };
}


