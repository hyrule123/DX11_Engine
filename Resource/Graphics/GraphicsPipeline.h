#pragma once
#include <Engine/Resource/Resource.h>

struct ID3D11DeviceContext;
struct ID3D11SamplerState;

namespace engine
{
    class InputLayout;
    class VertexShader;
    class PixelShader;

    class GraphicsPipeline :
        public Resource
    {
        CLASS_INFO(GraphicsPipeline, Resource)
    public:
        GraphicsPipeline();
        virtual ~GraphicsPipeline() override;

        void SetInputLayout(s_ptr<InputLayout> input_layout) { 
            input_layout_ = std::move(input_layout); 
        }

        void SetVertexShader(s_ptr<VertexShader> vertex_shader) {
            vs_ = std::move(vertex_shader);
        }

        void SetPixelShader(s_ptr<PixelShader> pixel_shader) {
            ps_ = std::move(pixel_shader);
        }

        void Bind(ID3D11DeviceContext* context);

    private:
        s_ptr<InputLayout> input_layout_ = {};
        s_ptr<VertexShader> vs_ = {};
        s_ptr<PixelShader> ps_ = {};


    };
}


