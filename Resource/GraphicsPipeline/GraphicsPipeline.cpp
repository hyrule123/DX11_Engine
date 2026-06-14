#include "Engine/Core/pch.h"
#include "GraphicsPipeline.h"

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/GraphicsPipeline/InputLayout.h>
#include <Engine/Resource/GraphicsPipeline/VertexShader.h>
#include <Engine/Resource/GraphicsPipeline/PixelShader.h>

namespace engine
{
	GraphicsPipeline::GraphicsPipeline()
		: Resource(STRINGIFY(GraphicsPipeline))
	{
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
	}
	void GraphicsPipeline::Bind()
	{
		auto context = GraphicsDevice::GetInst().GetContext();

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

