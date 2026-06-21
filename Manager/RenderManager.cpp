#include "Engine/Core/pch.h"
#include "RenderManager.h"

#include <Engine/Game/Component/Camera.h>
#include <Engine/Game/Component/Renderer.h>
#include <Engine/Game/Component/Transform.h>

#include <Engine/Resource/GPUBuffer/ConstantBuffer.h>
#include <Engine/Resource/Material/Material.h>
#include <Engine/Resource/Mesh/Mesh.h>

#include <Engine/HLSL/CppShared/Struct.hlsli>

#include <Engine/Manager/GraphicsDevice.h>

#include <Engine/Core/Debug.h>


namespace engine
{
	RenderManager::RenderManager()
	{

	}

	RenderManager::~RenderManager()
	{

	}
	void RenderManager::Init()
	{
		cb_per_obj_ = std::make_shared<ConstantBuffer>();
		cb_per_obj_->Create<PerObj>();

		cb_per_pass_ = std::make_shared<ConstantBuffer>();
		cb_per_pass_->Create<PerPass>();
	}
	void RenderManager::Render()
	{
		if (main_cam_.expired()) 
		{ 
			ERROR_MESSAGE("Main Camera 없음");
			return; 
		}

		auto context = GraphicsDevice::GetInst().GetContext();

		auto cam = main_cam_.lock();

		//Per Pass ( = Camera )
		PerPass per_pass_data = {};
		per_pass_data.view_mat = cam->GetViewMatrix();
		per_pass_data.proj_mat = cam->GetProjMatrix();

		cb_per_pass_->Upload(context.Get(), per_pass_data);
		cb_per_pass_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_PASS);

		//오브젝트 별 렌더링
		for (const auto& renderer : render_queue_)
		{
			if (false == renderer->IsRenderReady()) { continue; }

			//Per Obj
			const matrix& world = renderer->GetTransform()->GetWorldMatrix();
			cb_per_obj_->Upload(context.Get(), world);
			cb_per_obj_->Bind(context.Get(), ShaderStage::kAllGraphics, SLOT_B_PER_OBJECT);

			renderer->GetMaterial()->Bind(context.Get());
			renderer->GetMesh()->Render(context.Get());
		}
	}
	void RenderManager::FrameEnd()
	{
		render_queue_.clear();
	}
}