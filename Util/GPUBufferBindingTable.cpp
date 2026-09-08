#include "Engine/Core/pch.h"
#include "GPUBufferBindingTable.h"

#include <Engine/Resource/GPU/Buffer/ConstantBuffer.h>
#include <Engine/Resource/GPU/Buffer/ShaderResource.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	GPUBufferBindingTable::GPUBufferBindingTable(uint32 cb_slot_start, uint32 cb_slot_count, uint32 srv_slot_start, uint32 srv_slot_count)
		: cb_slot_start_(cb_slot_start)
		, cb_slot_count_(cb_slot_count)
		, srv_slot_start_(srv_slot_start)
		, srv_slot_count_(srv_slot_count)
	{
		ASSERT_RELEASE_MESSAGE(cb_slot_count_ <= kMaxBufferPerBindingTable && srv_slot_count_ <= kMaxBufferPerBindingTable, "버퍼 수가 최대 버퍼 수를 초과합니다.");

		ASSERT_RELEASE_MESSAGE((uint64)cb_slot_start_ + (uint64)cb_slot_count_ <= (uint64)D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "cb_slot_count가 D3D11 상수 버퍼 슬롯 수를 초과합니다.");
		ASSERT_RELEASE_MESSAGE((uint64)srv_slot_start_ + (uint64)srv_slot_count_ <= (uint64)D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, "srv_slot_count가 D3D11 입력 리소스 슬롯 수를 초과합니다.");
	}

	void GPUBufferBindingTable::AddConstantBuffer(ShaderStage::Flags stage_flag, uint32 slot, s_ptr<ConstantBuffer> buffer)
	{
		const uint32 idx = slot - cb_slot_start_;
		if (slot < cb_slot_start_ || idx >= cb_slot_count_)
		{
			ASSERT_MESSAGE(false, "CB 슬롯 범위 초과");
			return;
		}
		if (buffer == nullptr)
		{
			ASSERT_MESSAGE(false, "CB 버퍼가 nullptr입니다.");
			return;
		}
		
		// index만큼 cb_bindings가 존재하지 않으면 resize(검증 필요 없음)
		if (idx >= cb_bindings_.size())
		{
			cb_bindings_.resize(idx + 1);
		}
		// index만큼 cb_bindings가 존재하면 이미 등록된 버퍼가 있는지 확인
		else if(cb_bindings_[idx].buffer != nullptr)
		{
			ASSERT_MESSAGE(false, "이미 같은 슬롯에 버퍼가 등록되어 있습니다.");
			return;
		}

		cb_bindings_[idx] = { stage_flag, buffer };
	}

	void GPUBufferBindingTable::RemoveConstantBuffer(uint32 slot)
	{
		const uint32 idx = slot - cb_slot_start_;
		if (slot < cb_slot_start_ || idx >= cb_slot_count_)
		{
			ASSERT_MESSAGE(false, "CB 슬롯 범위 초과");
			return;
		}
		if (idx >= cb_bindings_.size() || cb_bindings_[idx].buffer == nullptr)
		{
			ASSERT_MESSAGE(false, "해당 슬롯에 등록된 CB 버퍼가 없습니다.");
			return;
		}
		cb_bindings_[idx] = { ShaderStage::Flags::None, nullptr };
	}

	void GPUBufferBindingTable::AddShaderResource(ShaderStage::Flags stage_flag, uint32 slot, s_ptr<ShaderResource> buffer)
	{
		const uint32 idx = slot - srv_slot_start_;
		if (slot < srv_slot_start_ || idx >= srv_slot_count_)
		{
			ASSERT_MESSAGE(false, "SRV 슬롯 범위 초과");
			return;
		}
		if (buffer == nullptr)
		{
			ASSERT_MESSAGE(false, "SRV 버퍼가 nullptr입니다.");
			return;
		}

		// index만큼 cb_bindings가 존재하지 않으면 resize(검증 필요 없음)
		if (idx >= srv_bindings_.size())
		{
			srv_bindings_.resize(idx + 1);
		}
		// index만큼 cb_bindings가 존재하면 이미 등록된 버퍼가 있는지 확인
		else if(srv_bindings_[idx].buffer != nullptr)
		{
			ASSERT_MESSAGE(false, "이미 같은 슬롯에 버퍼가 등록되어 있습니다.");
			return;
		}

		srv_bindings_[idx] = { stage_flag, buffer };
	}

	void GPUBufferBindingTable::RemoveShaderResource(uint32 slot)
	{
		const uint32 idx = slot - srv_slot_start_;
		if (slot < srv_slot_start_ || idx >= srv_slot_count_)
		{
			ASSERT_MESSAGE(false, "SRV 슬롯 범위 초과");
			return;
		}
		if (idx >= srv_bindings_.size() || srv_bindings_[idx].buffer == nullptr)
		{
			ASSERT_MESSAGE(false, "해당 슬롯에 등록된 SRV 버퍼가 없습니다.");
			return;
		}
		srv_bindings_[idx] = { ShaderStage::Flags::None, nullptr };
	}

	void GPUBufferBindingTable::BindCBImpl(ID3D11DeviceContext * context) const
	{
		if (cb_bindings_.empty()) { return; }

		// 실제로 사용된 ShaderStage 플래그를 계산
		ShaderStage::Flags used_stage_flags = ShaderStage::Flags::None;
		for (size_t i = 0; i < cb_bindings_.size(); ++i)
		{
			used_stage_flags |= cb_bindings_[i].stage_flag;
		}

		std::array<ID3D11Buffer*, kMaxBufferPerBindingTable> bind_ptr;
		for (uint8 i = 0; i < (uint8)ShaderStage::Enum::kEND; ++i)
		{
			const ShaderStage::Enum stage = static_cast<ShaderStage::Enum>(i);
			const ShaderStage::Flags stage_flag = ShaderStage::EnumToFlag(stage);

			// 실제 사용된 ShaderStage에 대해서만 바인딩 수행
			if (ShaderStage::HasFlag(used_stage_flags, stage_flag) == false) { continue; }
			
			// 각 ShaderStage에 대해 바인딩할 ConstantBuffer를 준비, 바인딩할 constant buffer의 최대 개수도 같이 계산
			uint32 max_cb_count = 0u;
			for (uint32 j = 0; j < (uint32)cb_bindings_.size(); ++j)
			{
				if (ShaderStage::HasFlag(cb_bindings_[j].stage_flag, stage_flag))
				{
					bind_ptr[j] = cb_bindings_[j].buffer->GetRawBuffer();
					max_cb_count = j + 1;
				}
				else
				{
					bind_ptr[j] = nullptr;
				}
			}

			switch(stage)
			{
				case ShaderStage::Enum::Vertex:
					context->VSSetConstantBuffers(cb_slot_start_, max_cb_count, bind_ptr.data());
					break;
				case ShaderStage::Enum::Geometry:
					context->GSSetConstantBuffers(cb_slot_start_, max_cb_count, bind_ptr.data());
					break;
				case ShaderStage::Enum::Pixel:
					context->PSSetConstantBuffers(cb_slot_start_, max_cb_count, bind_ptr.data());
					break;
				case ShaderStage::Enum::Compute:
					context->CSSetConstantBuffers(cb_slot_start_, max_cb_count, bind_ptr.data());
					break;
				default:
					ASSERT_MESSAGE(false, "Invalid shader stage");
					break;
			}
		}
	}

	void GPUBufferBindingTable::BindSRVImpl(ID3D11DeviceContext * context) const
	{
		if (srv_bindings_.empty()) { return; }

		// 실제로 사용된 ShaderStage 플래그를 계산
		ShaderStage::Flags used_stage_flags = ShaderStage::Flags::None;
		for (size_t i = 0; i < srv_bindings_.size(); ++i)
		{
			used_stage_flags |= srv_bindings_[i].stage_flag;
		}
		std::array<ID3D11ShaderResourceView*, kMaxBufferPerBindingTable> bind_ptr;
		for (uint8 i = 0; i < (uint8)ShaderStage::Enum::kEND; ++i)
		{
			const ShaderStage::Enum stage = static_cast<ShaderStage::Enum>(i);
			const ShaderStage::Flags stage_flag = ShaderStage::EnumToFlag(stage);
			// 실제 사용된 ShaderStage에 대해서만 바인딩 수행
			if (ShaderStage::HasFlag(used_stage_flags, stage_flag) == false) { continue; }
			uint32 max_srv_count = 0u;
			for (uint32 j = 0; j < (uint32)srv_bindings_.size(); ++j)
			{
				if (ShaderStage::HasFlag(srv_bindings_[j].stage_flag, stage_flag))
				{
					bind_ptr[j] = srv_bindings_[j].buffer->GetSRV();
					max_srv_count = j + 1;
				}
				else
				{
					bind_ptr[j] = nullptr;
				}
			}
			switch (stage)
			{
			case ShaderStage::Enum::Vertex:
				context->VSSetShaderResources(srv_slot_start_, max_srv_count, bind_ptr.data());
				break;
			case ShaderStage::Enum::Geometry:
				context->GSSetShaderResources(srv_slot_start_, max_srv_count, bind_ptr.data());
				break;
			case ShaderStage::Enum::Pixel:
				context->PSSetShaderResources(srv_slot_start_, max_srv_count, bind_ptr.data());
				break;
			case ShaderStage::Enum::Compute:
				context->CSSetShaderResources(srv_slot_start_, max_srv_count, bind_ptr.data());
				break;
			default:
				ASSERT_MESSAGE(false, "Invalid shader stage");
				break;
			}
		}
	}

}