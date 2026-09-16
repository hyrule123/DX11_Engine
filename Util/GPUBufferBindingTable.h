#pragma once
#include <Engine/Core/DX11.h>
#include <Engine/Core/SmartPointer.h>
#include <Engine/Core/Enum.h>

#include <vector>

// 한 번에 전체 바인딩 X, 
namespace engine
{
	class ConstantBuffer;
	class ShaderResource;

	template <typename T>
	struct BufferBindingInfo
	{
		ShaderStageFlags stage_flag;
		s_ptr<T> buffer = {};
	};

	class GPUBufferBindingTable
	{
		constexpr static uint32 kMaxBufferPerBindingTable = 10;
	public:
		GPUBufferBindingTable(uint32 cb_slot_start, uint32 cb_slot_count, uint32 srv_slot_start, uint32 srv_slot_count);
		GPUBufferBindingTable(const GPUBufferBindingTable&) = delete;
		GPUBufferBindingTable& operator=(const GPUBufferBindingTable&) = delete;
		GPUBufferBindingTable(GPUBufferBindingTable&&) = delete;
		GPUBufferBindingTable& operator=(GPUBufferBindingTable&&) = delete;

		void AddConstantBuffer(ShaderStageFlags stage_flag, RegisterB slot, s_ptr<ConstantBuffer> buffer);
		void RemoveConstantBuffer(RegisterB slot);

		void AddShaderResource(ShaderStageFlags stage_flag, RegisterT slot, s_ptr<ShaderResource> buffer);
		void RemoveShaderResource(RegisterT slot);

		void Bind(ID3D11DeviceContext* context) const {
			BindCBImpl(context);
			BindSRVImpl(context);
		}

	private:
		void BindCBImpl(ID3D11DeviceContext* context) const;
		void BindSRVImpl(ID3D11DeviceContext* context) const;

		const uint32 cb_slot_start_;
		const uint32 cb_slot_count_;
		const uint32 srv_slot_start_;
		const uint32 srv_slot_count_;
		std::vector<BufferBindingInfo<ConstantBuffer>> cb_bindings_;
		std::vector<BufferBindingInfo<ShaderResource>> srv_bindings_;
	};
}
