#pragma once
#include <Engine/Resource/Graphics/RenderPass/RenderPass.h>

#include <Engine/Core/StdType.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine
{
    class Mesh;
    class Material;
    class Renderer;
    class RenderManager;
    class StructuredBuffer;

    class ForwardOpaqueRenderPass final :
        public RenderPass
    {
		using Super = RenderPass;
    public:
        struct RenderItem
        {
            RenderKey key = {};
            Renderer* renderer = {};

			auto operator<=>(const RenderItem& other) const {
				return key <=> other.key;
			}
			auto operator == (const RenderItem& other) const {
				return key == other.key;
			}
        };

        ForwardOpaqueRenderPass();
		virtual ~ForwardOpaqueRenderPass() override;

        void SubmitRenderItem(const RenderItem& item) {
			render_queue_.push_back(item);
        }

		virtual void Execute( ID3D11DeviceContext* context) final;

    private:
		std::vector<RenderItem> render_queue_ = {};

		std::unordered_map<RenderKey, u_ptr<StructuredBuffer>, RenderKeyHasher> instancing_data_buffers_ = {};
    };
}


