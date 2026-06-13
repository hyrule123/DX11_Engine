#include "Engine/Core/pch.h"
#include "Scene.h"

namespace engine
{
	Scene::Scene(const std::string_view concrete_class_name)
		: Entity(concrete_class_name)
	{}
	Scene::~Scene()
	{}
}