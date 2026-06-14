#include "Engine/Core/pch.h"
#include "Renderer.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/Mesh/Mesh.h>
#include <Engine/Resource/GraphicsPipeline/InputLayout.h>
#include <Engine/Resource/GraphicsPipeline/VertexShader.h>
#include <Engine/Resource/GraphicsPipeline/PixelShader.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	Renderer::Renderer()
		: Component(STRINGIFY(Renderer), ComponentCategory::kRenderer)
	{

	}

	Renderer::~Renderer()
	{

	}
	void Renderer::Init()
	{
		Super::Init();

		auto& resmgr = ResourceManager::GetInst();

		input_layout_ = resmgr.Find<InputLayout>("Debug_IL");
		vs_ = resmgr.Find<VertexShader>("Shader/Debug_VS.cso");
		ps_ = resmgr.Find<PixelShader>("Shader/Debug_PS.cso");

		if (!(input_layout_ && vs_ && ps_))
		{
			ERROR_MESSAGE("뭔가 로딩 안됨");
		}
	}
	void Renderer::Render()
	{
		Super::Render();

		if (mesh_ && input_layout_ && vs_ && ps_)
		{
			auto context = GraphicsDevice::GetInst().GetContext();

			input_layout_->Bind(context);
			vs_->Bind(context);
			ps_->Bind(context);

			mesh_->Render();
		}
	}
}

