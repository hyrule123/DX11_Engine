#include "Engine/Core/pch.h"
#include "Scene.h"

namespace engine
{
	Scene::Scene(const std::string_view concrete_class_name)
		: Super(concrete_class_name)
	{}
	Scene::~Scene()
	{}
}