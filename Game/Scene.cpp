#include "Engine/pch.h"
#include "Scene.h"

namespace engine
{
	Scene::Scene(const std::string_view class_name)
		: Entity(class_name)
	{}
	Scene::~Scene()
	{}
}