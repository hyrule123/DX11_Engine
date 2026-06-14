#include "Engine/Core/pch.h"
#include "Renderer.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Resource/Mesh/Mesh.h>
#include <Engine/Resource/Material/Material.h>

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
	}
	void Renderer::Render()
	{
		Super::Render();

		if (mesh_ && material_)
		{
			auto context = GraphicsDevice::GetInst().GetContext();

			//
			material_->Bind();

			mesh_->Render();
		}
		else
		{
			DEBUG_LOG("Mesh 혹은 Material이 없어 렌더링 실패");
		}
	}
}

