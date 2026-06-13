#include "Engine/Core/pch.h"
#include "Entity.h"

namespace engine
{
	Entity::Entity(const std::string_view concrete_class_name)
		: concrete_class_name_(concrete_class_name)
		, id_(next_id_++)
	{}

	Entity::~Entity()
	{}
}

