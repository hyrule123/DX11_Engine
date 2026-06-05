#include "Engine/pch.h"
#include "Entity.h"

namespace engine
{
	Entity::Entity(const std::string_view class_name)
		: class_name_(class_name)
		, id_(next_id_++)
	{}

	Entity::~Entity()
	{}
}

