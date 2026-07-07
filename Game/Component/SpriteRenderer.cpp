#include "Engine/Core/pch.h"
#include "SpriteRenderer.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/RenderManager.h>

#include <Engine/Resource/Graphics/Material.h>
#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/Shader/VertexShader.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Core/Debug.h>

namespace engine
{
	SpriteRenderer::SpriteRenderer()
		: Super(SpriteRenderer::kClassConcreteName, sizeof(per_obj_data_))
	{}
	SpriteRenderer::~SpriteRenderer()
	{}

	void SpriteRenderer::Init()
	{
		Super::Init();
		bool result = SetMesh("Mesh_Standard2D_Rect");
		result = (result && SetMaterial("Material_Sprite"));

		ASSERT(result);
	}
	void SpriteRenderer::LateUpdate()
	{
		Super::LateUpdate();

		if (!IsRenderReady()) 
		{ 
			DEBUG_LOG("SpriteRenderer is not ready to render. Material or Mesh is missing.");
			return; 
		}

		auto* opaque_pass = RenderManager::GetInst().GetOpaquePass();
		
		ForwardOpaqueRenderPass::RenderItem item;
		item.key.material_id = GetMaterial()->GetInstanceID();
		item.key.mesh_id = GetMesh()->GetInstanceID();
		item.renderer = this;

		opaque_pass->SubmitRenderItem(item);
	}
	void SpriteRenderer::WritePerObjData(void* ptr)
	{
		per_obj_data_.world_mat = GetTransform()->GetWorldMatrix();
		memcpy(ptr, &per_obj_data_, sizeof(per_obj_data_));
	}
}