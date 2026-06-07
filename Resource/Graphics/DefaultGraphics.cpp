#include "Engine/pch.h"
#include "DefaultGraphics.h"

#include <Engine/Resource/Graphics/InputLayout.h>
#include <Engine/Resource/Graphics/VertexShader.h>
#include <Engine/Resource/Graphics/PixelShader.h>

namespace engine
{
	void DefaultGraphics::CreateDefaultGraphicsResource()
	{
		{
			D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
	{
		"POSITION",                     // 1. SemanticName: VS의 입력 구조체 매개변수 이름과 일치해야 합니다.
		0,                              // 2. SemanticIndex: POSITION0을 의미합니다. (뒤에 숫자가 없을 때 기본값 0)
		DXGI_FORMAT_R32G32B32_FLOAT,    // 3. Format: float 3개(X, Y, Z 각각 4바이트, 총 12바이트) 구조입니다.
		0,                              // 4. InputSlot: 0번 정점 버퍼 슬롯을 사용합니다.
		0,                              // 5. AlignedByteOffset: 구조체의 맨 처음(0바이트 지점)부터 데이터가 시작됩니다.
		D3D11_INPUT_PER_VERTEX_DATA,   // 6. InputSlotClass: 정점당(Per-Vertex) 데이터를 읽어옵니다.
		0                               // 7. InstanceDataStepRate: 인스턴싱을 쓰지 않으므로 0으로 설정합니다.
	}
			};
			// 원소 개수 안전하게 구하기
			UINT numElements = std::size(layoutDesc); // 혹은 ARRAYSIZE(layoutDesc)

			D3D11_INPUT_ELEMENT_DESC desc = {};
			desc.SemanticName = "POSITION";
			desc.SemanticIndex = 0;
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;


			std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
			
			desc.InputSlot = 
		}
	}
}

