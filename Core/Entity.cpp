#include "Engine/Core/pch.h"
#include "Entity.h"

namespace engine
{
	Entity::Entity(const HashedStringView& concrete_class_name)
		: concrete_class_name_(concrete_class_name)
		, instance_ID_(next_instance_ID_++)
	{}

	Entity::Entity(const Entity & copy)
		: concrete_class_name_(copy.concrete_class_name_)
		, instance_ID_(next_instance_ID_++)
	{}

	Entity::~Entity()
	{}

	EntityManager::EntityManager() {}
	EntityManager::~EntityManager() {}

	u_ptr<Entity> EntityManager::CreateEntity(const HashedStringView& key)
	{
		auto it = entity_creator_table_.find(key);
		if (it != entity_creator_table_.end())
		{
			// EntityCreationFunc func = it->second;
			return it->second();
		}
		return nullptr;
	}
}
