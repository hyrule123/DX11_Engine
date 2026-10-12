#include "Engine/Core/pch.h"
#include "InputLayout.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Resource/GPU/Shader/VertexShader.h>

#include <Engine/Resource/GPU/Vertex.h>

namespace engine
{
	InputLayout::InputLayout()
		: Super(InputLayout::kClassConcreteName)
		, input_layout_()
		, has_instance_id_()
	{}
	InputLayout::~InputLayout()
	{}
	bool InputLayout::Create(std::span<const D3D11_INPUT_ELEMENT_DESC> layout_descs, VertexShader* vs)
	{
		if (layout_descs.empty())
		{
			ERR_MSG("InputLayoutDesc is empty.");
			return false;
		}
		if (vs == nullptr || vs->IsReady() == false)
		{
			ERR_MSG("VertexShader is not ready.");
			return false;
		}

		// 사용자 desc 검사: 예약 슬롯/시맨틱 사용 금지
		for (const D3D11_INPUT_ELEMENT_DESC& desc : layout_descs)
		{
			ASSERT_F(desc.InputSlot != vertex::instance_id::kInputSlot, "슬롯 31은 INSTANCE_ID 전용입니다.");
			ASSERT_F(_stricmp(desc.SemanticName, vertex::instance_id::kSemanticName) != 0, "INSTANCE_ID 시맨틱은 엔진이 자동으로 추가합니다.");
		}

		// 고정 크기 배열로 복사 후 필요 시 추가 (힙 할당 없음)
		std::array<D3D11_INPUT_ELEMENT_DESC, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT> descs;

		// 필요 슬롯 수 검사
		const size_t required = layout_descs.size() + (vs->UsesInstanceID() ? 1 : 0);
		if (required > descs.size())
		{
			ERR_MSG("InputLayout 원소 수가 D3D11 한도(32)를 초과합니다.");
			return false;
		}

		std::copy(layout_descs.begin(), layout_descs.end(), descs.begin());
		UINT desc_count = (UINT)layout_descs.size();

		if (vs->UsesInstanceID())
		{
			descs[desc_count++] = vertex::instance_id::kInputLayoutDesc;
		}

		auto* device = GraphicsDevice::GetInst().GetDevice();

		HRESULT hr = device->CreateInputLayout(
			descs.data(),
			desc_count,
			vs->GetByteCode().data(),
			vs->GetByteCode().size(),
			input_layout_.GetAddressOf()
		);

		if (FAILED(hr))
		{
			ERR_MSG_HRESULT(hr);
			return false;
		}

		return true;
	}

	void InputLayout::Bind(ID3D11DeviceContext* context) const
	{
		ASSERT(input_layout_ != nullptr);
		context->IASetInputLayout(input_layout_.Get());
	}
}