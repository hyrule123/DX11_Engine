#include "Engine/Core/pch.h"
#include "SpriteRenderer.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Resource/GPU/Material.h>
#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/Shader/VertexShader.h>
#include <Engine/Resource/GPU/PipelineState.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Core/Debug.h>

#include <Engine/HLSL/DebugDraw/DebugDraw.hlsli>

namespace engine
{
	SpriteRenderer::SpriteRenderer()
		: Super(SpriteRenderer::kClassConcreteName)
	{}
	SpriteRenderer::~SpriteRenderer()
	{}

	void SpriteRenderer::Init()
	{
		constexpr HashedStringView hsh = "Mesh_Standard2D_Rect";

		Super::Init();
		bool result = SetMesh("Mesh_Standard2D_Rect");
		ASSERT(result);
		result = SetMaterial(0, "Material_Sprite");
		ASSERT(result);
	}
	void SpriteRenderer::Awake()
	{
		Super::Awake();

		Material* mtrl = GetMaterial(0);
		ASSERT(mtrl);
		PipelineState* ps = mtrl->GetPipelineState(render_pass_mode_);
		ASSERT(ps);
		ASSERT(sizeof(per_obj_data_) == ps->GetInstanceDataStride());
	}

	/*
	void SpriteRenderer::LateUpdate()
	{
		Super::LateUpdate();

		if (!IsRenderReady()) 
		{ 
			DEBUG_LOG("SpriteRenderer is not ready to render. Material or Mesh is missing.");
			return; 
		}

		if (render_pass_mode_ == render_pass::ID::ForwardOpaque
			&&
			GetMaterial(0)->IsReady(render_pass_mode_))
		{
			auto* opaque_pass = RenderManager::GetInst().GetOpaquePass();

			ForwardOpaquePass::RenderItem item;
			item.key.material_id = GetMaterial(0)->GetInstanceID();
			item.key.mesh_id = GetMesh()->GetInstanceID();
			item.renderer = this;

			opaque_pass->SubmitRenderItem(item);
		}
	}
	*/

	void SpriteRenderer::WriteInstanceData(DataBlock data_block)
	{
		per_obj_data_.world_mat = GetTransform()->GetWorldMatrix();

		ASSERT(data_block.IsValid());
		data_block.Write(per_obj_data_);

//#ifndef NDEBUG
//		DebugInstanceData debug_data = {};
//		debug_data.world_mat = per_obj_data_.world_mat;
//		debug_data.color = { 0.0f, 1.0f, 0.0f, 1.0f };
//		RenderManager::GetInst().DrawDebugRect(debug_data);
//#endif
	}
}