#pragma once
#include <Engine/Resource/Resource.h>

struct ID3D11DeviceContext;
struct ID3D11SamplerState;

namespace engine
{
    class InputLayout;
    class VertexShader;
    class PixelShader;
    class RasterizerState;
    class BlendState;
    class DepthStencilState;

    class GraphicsPipeline :
        public Resource
    {
        CLASS_INFO(GraphicsPipeline, Resource)
    public:
        GraphicsPipeline();
        virtual ~GraphicsPipeline() override;

        bool SetInputLayout(const stdfs::path& layout_name);
        bool SetInputLayout(
            const std::vector<D3D11_INPUT_ELEMENT_DESC>& il_desc, 
            const stdfs::path& vs_path
        );
        void SetInputLayout(s_ptr<InputLayout> input_layout) { 
            input_layout_ = std::move(input_layout); 
        }

        bool SetVertexShader(const stdfs::path& vs_name);
        void SetVertexShader(s_ptr<VertexShader> vertex_shader) {
            vertex_shader_ = std::move(vertex_shader);
        }

        bool SetPixelShader(const stdfs::path& ps_name);
        void SetPixelShader(s_ptr<PixelShader> pixel_shader) {
            pixel_shader_ = std::move(pixel_shader);
        }

        bool SetRasterizerState(const stdfs::path& rss_name);
        void SetRasterizerState(s_ptr<RasterizerState> rss) {
            rasterizer_state_ = rss;
        }

        bool SetBlendState(const stdfs::path& bs_name);
        void SetBlendState(s_ptr<BlendState> bs) {
            blend_state_ = bs;
        }

        bool SetDepthStencilState(const stdfs::path& ds_name);
        void SetDepthStencilState(s_ptr<DepthStencilState> dss) {
            depth_stencil_state_ = dss;
        }

        void Bind(ID3D11DeviceContext* context);

    private:
        s_ptr<InputLayout> input_layout_ = {};
        s_ptr<VertexShader> vertex_shader_ = {};
        s_ptr<PixelShader> pixel_shader_ = {};
        
        s_ptr<RasterizerState> rasterizer_state_ = {};
        s_ptr<BlendState> blend_state_ = {};

        s_ptr<DepthStencilState> depth_stencil_state_ = {};
    };
}


