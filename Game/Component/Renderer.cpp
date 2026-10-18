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
		, renderer_id_(kInvalid32)
	{
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
		RenderManager::GetInst().RegisterRenderer(this);
	}
	void Renderer::OnDisable()
	{
		Super::OnDisable();
		Unsubscribe(SubscribeType::kTransformDirty);
		RenderManager::GetInst().UnregisterRenderer(this);
	}

	void Renderer::OnTransformDirty(Transform* transform)
	{
		Super::OnTransformDirty(transform);
		RenderManager::GetInst().MarkBoundsDirty(this);
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
		if (mesh_)
		{	
			submeshes_.resize(mesh_->GetSubMeshCount());
			for (size_t i = 0; i < submeshes_.size(); ++i)
			{
				BuildSubMeshRenderData(i);
			}
		}
		else
		{
			submeshes_.clear();
		}

		// 이미 등록되었다면 Bounds 재계산 필요
		if (renderer_id_ != kInvalid32)
		{
			RenderManager::GetInst().MarkBoundsDirty(this);
		}
	}

	bool Renderer::SetMaterial(size_t submesh_idx, std::shared_ptr<Material> material)
	{
		if (submesh_idx >= submeshes_.size())
		{
			ASSERT_F(false, "submesh index가 범위를 벗어남");
			return false;
		}

		submeshes_[submesh_idx].material = std::move(material);

		BuildSubMeshRenderData(submesh_idx);

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

	AABB3D Renderer::ComputeWorldBounds() const
	{
		if (mesh_ == nullptr) { return AABB3D::Invalid(); }

		//Arvo 알고리즘
		matrix world_mat = my_transform_->GetWorldMatrix();
		const AABB3D& local = local_bounds_override_.IsValid()
			? local_bounds_override_
			: mesh_->GetLocalBounds();

		const float3 center = (local.min + local.max) * 0.5f;
		const float3 extent = (local.max - local.min) * 0.5f;

		const float3 world_center = float3::Transform(center, world_mat);

		// 행 벡터 규약(v * M) 기준: 출력 축 j의 반지름 = Σ_i |M[i][j]| * extent[i]
		/* 기존 식
		const float3 world_extent(
			std::abs(world_mat._11) * extent.x + std::abs(world_mat._21) * extent.y + std::abs(world_mat._31) * extent.z,
			std::abs(world_mat._12) * extent.x + std::abs(world_mat._22) * extent.y + std::abs(world_mat._32) * extent.z,
			std::abs(world_mat._13) * extent.x + std::abs(world_mat._23) * extent.y + std::abs(world_mat._33) * extent.z);
		*/
		
		// 3*3 부분을 절대값으로 변환
		for (int i = 0; i < 3; ++i)
		{
			world_mat.m[i][0] = std::abs(world_mat.m[i][0]);
			world_mat.m[i][1] = std::abs(world_mat.m[i][1]);
			world_mat.m[i][2] = std::abs(world_mat.m[i][2]);
		}

		const float3 world_extent = float3::TransformNormal(extent, world_mat);

		AABB3D world_bounds;
		world_bounds.min = world_center - world_extent;
		world_bounds.max = world_center + world_extent;
		return world_bounds;
	}

	void Renderer::SetLocalBoundsOverride(const AABB3D& local_bounds)
	{
		if (local_bounds_override_ == local_bounds) { return; }

		local_bounds_override_ = local_bounds;

		if (IsRegistered())
		{
			RenderManager::GetInst().MarkBoundsDirty(this);
		}
	}

	void Renderer::BuildSubMeshRenderData(size_t submesh_idx)
	{
		ASSERT_F(submesh_idx < submeshes_.size(), "idx out of range");

		SubMesh& submesh = submeshes_[submesh_idx];

		if (mesh_ && submesh.material)
		{
			submesh.key = RenderKey(submesh.material->GetMaterialID(), mesh_->GetMeshID(), (uint8_t)submesh_idx);
		}
		else
		{
			submesh.key.Invalidate();
		}
	}
}

