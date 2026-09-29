#include "Engine/Core/pch.h"
#include "Renderer.h"

#include <Engine/Manager/RenderManager.h>
#include <Engine/Manager/ResourceManager.h>

#include <Engine/Resource/GPU/Mesh.h>
#include <Engine/Resource/GPU/Material.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/Debug.h>
#include <Engine/Core/Constants.h>

#include <Engine/Collision/Geometry2D.h>

namespace engine
{
	Renderer::Renderer(const HashedStringView& concrete_class_name)
		: Component(concrete_class_name, ComponentCategory::kRenderer)
	{
		ClearRenderSlots();
	}

	Renderer::~Renderer()
	{

	}
	void Renderer::Awake()
	{
		Super::Awake();
		my_transform_ = GetComponent<Transform>();
	}
	void Renderer::OnEnable()
	{
		Super::OnEnable();
		Subscribe(SubscribeType::kTransformDirty);
		RenderManager::GetInst().RefreshRenderer(this);
	}
	void Renderer::OnDisable()
	{
		Super::OnDisable();
		Unsubscribe(SubscribeType::kTransformDirty);
		RenderManager::GetInst().UnregisterRenderer(this);
	}

	bool Renderer::SetMesh(const HashedStringView& mesh_name)
	{
		std::shared_ptr<Mesh> mesh = ResourceManager::GetInst().Find<Mesh>(mesh_name);
		if (mesh)
		{
			SetMesh(mesh);
			return true;
		}
		return false;
	}

	void Renderer::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		mesh_ = std::move(mesh);
		materials_.clear();
		if (mesh_)
		{	
			materials_.resize(mesh_->GetSubMeshCount());
		}

		BuildSubMeshRenderData();
	}

	bool Renderer::SetMaterial(size_t submesh_idx, std::shared_ptr<Material> material)
	{
		if (mesh_ == nullptr)
		{
			ERR_MSG("Mesh를 먼저 설정하세요.");
			return false;
		}
		if (submesh_idx >= materials_.size())
		{
			ASSERT_F(false, "submesh index가 범위를 벗어남");
			return false;
		}

		materials_[submesh_idx] = std::move(material);

		BuildSubMeshRenderData();

		return true;
	}

	bool Renderer::SetMaterial(size_t submesh_idx, const HashedStringView& mtrl_name)
	{
		std::shared_ptr<Material> material = ResourceManager::GetInst().Find<Material>(mtrl_name);
		if (material)
		{
			SetMaterial(submesh_idx, material);
			return true;
		}
		return false;
	}

	AABB2D Renderer::ComputeWorldBounds2D() const
	{
		ASSERT(mesh_ && my_transform_);

		const AABB3D& local_bounds = mesh_->GetLocalBounds();
		const matrix world_mat = my_transform_->GetWorldMatrix();

		return geometry_2d::TransformBoundsTo2D(local_bounds, world_mat);
	}
	void Renderer::BuildSubMeshRenderData()
	{
		bool needs_refresh = (submesh_render_data_.empty() == false);

		submesh_render_data_.assign(materials_.size(), SubMeshRenderData{});   // kInvalidKey로 채움

		if (mesh_ == nullptr) { return; }

		for (size_t i = 0; i < materials_.size(); ++i) 
		{
			if (materials_[i] == nullptr) { continue; }

			const auto& pso_per_pass = materials_[i]->GetPipelineStates();

			RenderPassFlags pass_flags = {};
			for (size_t j = 0; j < pso_per_pass.size(); ++j)
			{
				if (pso_per_pass[j])
				{
					pass_flags.Set((RenderPassOrder)j);
				}
			}
			submesh_render_data_[i] = SubMeshRenderData{RenderKey(materials_[i]->GetMaterialID(), mesh_->GetMeshID(), (uint8)i), pass_flags};
		}

		if (needs_refresh)
		{
			RenderManager::GetInst().RefreshRenderer(this);
		}
	}
}

