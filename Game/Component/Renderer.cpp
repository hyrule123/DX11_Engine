#include "Engine/Core/pch.h"
#include "Renderer.h"

#include <Engine/Manager/GraphicsDevice.h>
#include <Engine/Manager/RenderManager.h>

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
}

