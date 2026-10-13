#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>

#include <Engine/Util/GPUBufferBindingTable.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>
#include <Engine/Core/DX11.h>

#include <span>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace engine
{
    struct RenderContext;
    class InputLayout;
    class VertexShader;
    class PixelShader;
    class RasterizerState;
    class BlendState;
    class DepthStencilState;

    class PipelineState :
        public Resource
    {
        ENTITY_INFO(PipelineState, Resource)
    public:
        PipelineState();
        virtual ~PipelineState() override;

		bool IsReady() const { return (bool)input_layout_ && (bool)vertex_shader_; }

		void SetPerInstanceDataStride(size_t stride) { per_instance_data_stride_ = stride; }
		template <typename T>
		void SetPerInstanceDataStride() { per_instance_data_stride_ = sizeof(T); }

		size_t GetPerInstanceDataStride() const { return per_instance_data_stride_; }

        bool SetVertexShader(const HashedStringView& vs_name);
        void SetVertexShader(std::shared_ptr<VertexShader> vertex_shader);
		std::shared_ptr<VertexShader> GetVertexShader() const { return vertex_shader_; }

		void SetInputLayout(std::shared_ptr<InputLayout> input_layout) { input_layout_ = std::move(input_layout); }
        bool SetInputLayout(const HashedStringView& name);
		std::shared_ptr<InputLayout> GetInputLayout() const { return input_layout_; }

        bool CreateInputLayout(std::span<const D3D11_INPUT_ELEMENT_DESC> layout_descs);

        bool SetPixelShader(const HashedStringView& ps_name);
        void SetPixelShader(std::shared_ptr<PixelShader> pixel_shader) {
			pixel_shader_ = std::move(pixel_shader);
        }
		std::shared_ptr<PixelShader> GetPixelShader() const { return pixel_shader_; }

        bool SetRasterizerState(const HashedStringView& rss_name);
        void SetRasterizerState(std::shared_ptr<RasterizerState> rss) {
			rasterizer_state_ = std::move(rss);
        }

        bool SetBlendState(const HashedStringView& bs_name);
        void SetBlendState(std::shared_ptr<BlendState> bs) {
			blend_state_ = std::move(bs);
        }

        bool SetDepthStencilState(const HashedStringView& ds_name);
        void SetDepthStencilState(std::shared_ptr<DepthStencilState> dss) {
			depth_stencil_state_ = std::move(dss);
        }

        void Bind(ID3D11DeviceContext* context) const;
		static void ClearAllBindings(ID3D11DeviceContext* context);

        void AddConstantBufferBinding(ShaderStageFlags stage_flag, RegisterB slot, std::shared_ptr<ConstantBuffer> constant_buffer) {
			per_pipeline_buffer_binding_table_.AddConstantBuffer(stage_flag, slot, constant_buffer);
        }
        void RemoveConstantBufferBinding(RegisterB slot) {
			per_pipeline_buffer_binding_table_.RemoveConstantBuffer(slot);
        }

		void AddShaderResourceBinding(ShaderStageFlags stage_flag, RegisterT slot, std::shared_ptr<ShaderResource> shader_resource) {
			per_pipeline_buffer_binding_table_.AddShaderResource(stage_flag, slot, shader_resource);
		}
        void RemoveShaderResourceBinding(RegisterT slot) {
			per_pipeline_buffer_binding_table_.RemoveShaderResource(slot);
		}

    private:
        std::shared_ptr<InputLayout> input_layout_ = {};

        std::shared_ptr<VertexShader> vertex_shader_ = {};
        std::shared_ptr<PixelShader> pixel_shader_ = {};
        
        std::shared_ptr<RasterizerState> rasterizer_state_ = {};
        std::shared_ptr<BlendState> blend_state_ = {};

        std::shared_ptr<DepthStencilState> depth_stencil_state_ = {};

        // Per Pipeline State
        GPUBufferBindingTable per_pipeline_buffer_binding_table_;

        // Per Instance
		size_t per_instance_data_stride_ = {};
    };
}


