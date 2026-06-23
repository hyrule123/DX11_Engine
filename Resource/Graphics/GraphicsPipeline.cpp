#include "Engine/Core/pch.h"
#include "GraphicsPipeline.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/Graphics/InputLayout.h>
#include <Engine/Resource/Graphics/VertexShader.h>
#include <Engine/Resource/Graphics/PixelShader.h>

namespace engine
{
	GraphicsPipeline::GraphicsPipeline()
		: Resource(STRINGIFY(GraphicsPipeline))
	{
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
	}
	void GraphicsPipeline::Bind(ID3D11DeviceContext* context)
	{
		if (input_layout_)
		{
			input_layout_->Bind(context);
		}
		else
		{
			context->IASetInputLayout(nullptr);
		}
		if (vs_)
		{
			vs_->Bind(context);
		}
		else
		{
			context->VSSetShader(nullptr, nullptr, 0u);
		}
		if (ps_)
		{
			ps_->Bind(context);
		}
		else
		{
			context->PSSetShader(nullptr, nullptr, 0u);
		}
		
	}
}

