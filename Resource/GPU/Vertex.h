#pragma once

#include <Engine/Core/Math.h>
#include <Engine/Core/DX11.h>

#include <Engine/HLSL/Debug/Debug.hlsli>
#include <Engine/HLSL/Sprite/Sprite.hlsli>

#include <array>

namespace engine
{
	namespace Vertex
	{
		namespace Debug
		{
			using Vertex = DebugVSInput;

			constexpr auto kInputLayoutDescs = []() {
				std::array<D3D11_INPUT_ELEMENT_DESC, 1> descs;
				//VS의 입력 구조체 매개변수 이름과 일치해야 함
				descs[0].SemanticName = "POSITION";

				//POSITION0을 의미 (뒤에 숫자가 없을 때 기본값 0)
				descs[0].SemanticIndex = 0;

				//데이터 포맷
				descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

				//InputSlot: 0번 정점 버퍼 슬롯을 사용합니다.
				descs[0].InputSlot = 0;

				//AlignedByteOffset: 구조체 시작점으로부터 몇 번째부터 읽어야 하는지
				descs[0].AlignedByteOffset = 0;

				//정점별로 데이터를 보낼것인지 인스턴스별로 보낼것인지
				descs[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

				//인스턴스 별로 보낼 때 인스턴스 별로 몇단계씩 올릴 것인지
				//e.g) 2 -> 0, 2, 4, 8, ...
				descs[0].InstanceDataStepRate = 0;

				return descs;
			}();
		}


		namespace Standard2D
		{
			using Vertex = SpriteVSInput;

			constexpr auto kInputLayoutDescs = []() {
				std::array<D3D11_INPUT_ELEMENT_DESC, 2> descs;

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

				return descs;
			}();
		}
	}
}