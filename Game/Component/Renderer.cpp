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
	Renderer::Renderer(const std::string_view concrete_class_name, size_t per_obj_data_size)
		: Component(concrete_class_name, ComponentCategory::kRenderer)
		, per_instance_data_size_(per_obj_data_size)
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

}

