#pragma once
#include <Engine/Core/Math.h>
#include <Engine/Core/DX11.h>

#include <Engine/HLSL/Sprite/Sprite.hlsli>

#include <array>

namespace engine
{
	namespace vertex
	{
		// MeshRenderPass를 사용하는 쉐이더는 INSTANCE_ID 입력 필요
		// 레이아웃에 이 desc를 포함시키면 됨.
		namespace instance_id
		{
			constexpr uint kInputSlot = 31; // 인스턴스 데이터는 일반적으로 다른 슬롯에 위치
			constexpr const char* kSemanticName = "INSTANCE_ID";

			static_assert(kInputSlot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, "입력 슬롯 범위 초과");

			constexpr auto kInputLayoutDesc = []() {
				D3D11_INPUT_ELEMENT_DESC desc{};
				desc.SemanticName = kSemanticName;
				desc.SemanticIndex = 0;
				desc.Format = DXGI_FORMAT_R32_UINT;
				desc.InputSlot = kInputSlot;
				desc.AlignedByteOffset = 0;
				desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
				desc.InstanceDataStepRate = 1; // 인스턴스마다 증가
				return desc;
				}();
		}

		namespace standard_2d
		{
			using Vertex = SpriteVSInput;

			constexpr auto kInputLayoutDescs = []() {
				std::array<D3D11_INPUT_ELEMENT_DESC, 3> descs{};

				descs[0].SemanticName = "POSITION";
				descs[0].SemanticIndex = 0;
				descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				descs[0].InputSlot = 0;
				descs[0].AlignedByteOffset = 0;
				descs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descs[0].InstanceDataStepRate = 0;

				descs[1].SemanticName = "TEXCOORD";
				descs[1].SemanticIndex = 0;
				descs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
				descs[1].InputSlot = 0;
				//참고: 같은 InputSlot 사용 시 D3D11_APPEND_ALIGNED_ELEMENT로 자동 정렬 가능
				descs[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				descs[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				descs[1].InstanceDataStepRate = 0;

				// instance ID 사용
				descs[2] = instance_id::kInputLayoutDesc;

				return descs;
			}();
		}


	}
}