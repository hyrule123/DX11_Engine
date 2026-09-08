#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Resource/GPU/Buffer/ShaderResource.h>

#include <Engine/Util/GPUBufferBindingTable.h>

#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Enum.h>
#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace engine
{
    class InputLayoutDesc;
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
		size_t GetPerInstanceDataStride() const { return per_instance_data_stride_; }
		void SetInstancingSupport(bool is_supported) { is_instancing_supported_ = is_supported; }
		bool IsInstancingSupported() const { return is_instancing_supported_; }

        bool SetVertexShader(const HashedStringView& vs_name);
        void SetVertexShader(s_ptr<VertexShader> vertex_shader);

        bool CreateInputLayout(const HashedStringView& layout_desc_name);
        bool CreateInputLayout(InputLayoutDesc* input_layout_desc);

        bool SetPixelShader(const HashedStringView& ps_name);
        void SetPixelShader(s_ptr<PixelShader> pixel_shader) {
			pixel_shader_ = std::move(pixel_shader);
        }

        bool SetRasterizerState(const HashedStringView& rss_name);
        void SetRasterizerState(s_ptr<RasterizerState> rss) {
			rasterizer_state_ = std::move(rss);
        }

        bool SetBlendState(const HashedStringView& bs_name);
        void SetBlendState(s_ptr<BlendState> bs) {
			blend_state_ = std::move(bs);
        }

        bool SetDepthStencilState(const HashedStringView& ds_name);
        void SetDepthStencilState(s_ptr<DepthStencilState> dss) {
			depth_stencil_state_ = std::move(dss);
        }

        void Bind(ID3D11DeviceContext* context);
		static void Clear(ID3D11DeviceContext* context);

        void AddConstantBufferBinding(ShaderStage::Flags stage_flag, uint32 slot, s_ptr<ConstantBuffer> constant_buffer) {
			per_pipeline_buffer_binding_table_.AddConstantBuffer(stage_flag, slot, constant_buffer);
        }
        void RemoveConstantBufferBinding(uint32 slot) {
			per_pipeline_buffer_binding_table_.RemoveConstantBuffer(slot);
        }

		void AddShaderResourceBinding(ShaderStage::Flags stage_flag, uint32 slot, s_ptr<ShaderResource> shader_resource) {
			per_pipeline_buffer_binding_table_.AddShaderResource(stage_flag, slot, shader_resource);
		}
        void RemoveShaderResourceBinding(uint32 slot) {
			per_pipeline_buffer_binding_table_.RemoveShaderResource(slot);
		}

    private:
        ComPtr<ID3D11InputLayout> input_layout_ = {};

        s_ptr<VertexShader> vertex_shader_ = {};
        s_ptr<PixelShader> pixel_shader_ = {};
        
        s_ptr<RasterizerState> rasterizer_state_ = {};
        s_ptr<BlendState> blend_state_ = {};

        s_ptr<DepthStencilState> depth_stencil_state_ = {};

        // Per Pipeline State
        GPUBufferBindingTable per_pipeline_buffer_binding_table_;

        // Per Instance
		size_t per_instance_data_stride_ = {};
        bool is_instancing_supported_ = {};
    };
}


