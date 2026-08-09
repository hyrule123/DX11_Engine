#include "Engine/Core/pch.h"
#include "SceneEntity.h"

namespace engine
{
	SceneEntity::SceneEntity(const HashedStringView& concrete_class_name)
		: Entity(concrete_class_name)
	{}
	SceneEntity::~SceneEntity()
	{}
}