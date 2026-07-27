#include "Engine/Core/pch.h"
#include "Entity.h"

namespace engine
{
	Entity::Entity(const HashedStringView& concrete_class_name)
		: concrete_class_name_(concrete_class_name)
		, instance_ID_(next_instance_ID_++)
	{}

	Entity::Entity(const Entity& copy)
		: concrete_class_name_(copy.concrete_class_name_)
		, instance_ID_(next_instance_ID_++)
	{}

	Entity::Entity(const Entity && move) noexcept
		: concrete_class_name_(move.concrete_class_name_)
		, instance_ID_(next_instance_ID_++)
	{}

	Entity::~Entity()
	{}


	EntityFactory::EntityFactory()
	{}

	EntityFactory::~EntityFactory()
	{}
}

