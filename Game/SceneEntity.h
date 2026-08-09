#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Core/HandlePointer.h>

namespace engine
{
	class SceneEntity
		: public Entity
		, public enable_handle_from_this<SceneEntity>
	{
		ENTITY_INFO(SceneEntity, Entity)
	public:
		SceneEntity(const HashedStringView& concrete_class_name);
		virtual ~SceneEntity() override;
	};
}