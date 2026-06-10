#include "Engine/Core/pch.h"
#include "DefaultScene.h"

#include <Engine/Manager/ResourceManager.h>
#include <Engine/Resource/Mesh/Mesh.h>

namespace engine
{
	DefaultScene::DefaultScene()
		: Scene("DefaultScene")
	{
	}
	DefaultScene::~DefaultScene()
	{
	}
	void DefaultScene::Init()
	{
		Scene::Init();

		test_mesh_ = ResourceManager::GetInst().Find<Mesh>("DebugMesh");
	}
	void DefaultScene::Render()
	{
		

	}
}
