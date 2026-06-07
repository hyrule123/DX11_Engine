#include "Engine/pch.h"
#include "DefaultGraphicsRes.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/Graphics/InputLayout.h>
#include <Engine/Resource/Graphics/VertexShader.h>
#include <Engine/Resource/Graphics/PixelShader.h>

namespace engine
{
	void DefaultGraphicsRes::Load()
	{
		auto& resmgr = ResourceManager::GetInst();
		auto vs = resmgr.LoadFromFile<VertexShader>("Shader/Debug_VS.cso");

		{
			D3D11_INPUT_ELEMENT_DESC desc = {};

			//VS의 입력 구조체 매개변수 이름과 일치해야 함
			desc.SemanticName = "POSITION";

			//POSITION0을 의미 (뒤에 숫자가 없을 때 기본값 0)
			desc.SemanticIndex = 0;

			//데이터 포맷
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

			//InputSlot: 0번 정점 버퍼 슬롯을 사용합니다.
			desc.InputSlot = 0;

			//AlignedByteOffset: 구조체 시작점으로부터 몇 번째부터 읽어야 하는지
			desc.AlignedByteOffset = 0;

			//정점별로 데이터를 보낼것인지 인스턴스별로 보낼것인지
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

			//인스턴스 별로 보낼 때 인스턴스 별로 몇단계씩 올릴 것인지
			//e.g) 2 -> 0, 2, 4, 8, ...
			desc.InstanceDataStepRate = 0;

			std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
			descs.push_back(desc);
			
			auto il = std::make_shared<InputLayout>();
			il->Create(descs, vs);
		}
	}
}

