#include "Engine/Core/pch.h"
#include "DefaultScene.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/Mesh/Mesh.h>

namespace engine
{
	DefaultScene::DefaultScene()
		: Super(CLASS_TO_STRING(DefaultScene))
	{
	}
	DefaultScene::~DefaultScene()
	{
	}
	void DefaultScene::Init()
	{
		Super::Init();

		test_mesh_ = ResourceManager::GetInst().Find<Mesh>("DebugMesh");
	}
	void DefaultScene::Render()
	{
		

	}
}
