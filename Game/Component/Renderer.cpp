#include "Engine/Core/pch.h"
#include "Renderer.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/RenderManager.h>
#include <Engine/Manager/ResourceManager.h>
#include <Engine/Manager/TimeManager.h>

#include <Engine/Resource/Graphics/Mesh.h>
#include <Engine/Resource/Graphics/Material.h>

#include <Engine/Game/Component/Transform.h>

#include <Engine/Core/UtilMacro.h>
#include <Engine/Core/Debug.h>

namespace engine
{
	Renderer::Renderer()
		: Component(STRINGIFY(Renderer), ComponentCategory::kRenderer)
	{

	}

	Renderer::Renderer(const std::string_view concrete_class_name)
		: Component(concrete_class_name, ComponentCategory::kRenderer)
	{}

	Renderer::~Renderer()
	{

	}
	void Renderer::Init()
	{
		Super::Init();
	}
	void Renderer::Awake()
	{
		Super::Awake();

		my_transform_ = GetComponent<Transform>();
	}
	void Renderer::LateUpdate()
	{
		Super::LateUpdate();

		s_ptr<Renderer> me = std::static_pointer_cast<Renderer>(shared_from_this());
		RenderManager::GetInst().AddRenderQueue(me);
	}
	bool Renderer::SetMaterial(const stdfs::path& mtrl_name)
	{
		material_ = ResourceManager::GetInst().Find<Material>(mtrl_name);
		return (nullptr != material_);
	}
	bool Renderer::SetMesh(const stdfs::path & mesh_name)
	{
		mesh_ = ResourceManager::GetInst().Find<Mesh>(mesh_name);
		return (nullptr != mesh_);
	}
	PerObj Renderer::GetPerObjData()
	{
		PerObj per_obj = {};
		per_obj.world_mat = my_transform_->GetWorldMatrix();

		return per_obj;
	}
}

