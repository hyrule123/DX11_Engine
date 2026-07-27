#pragma once

#include <Engine/Core/Entity.h>

#include <Engine/Core/UtilMacro.h>



namespace engine
{
	class Resource 
		: public Entity
	{
		CLASS_INFO(Resource, Entity)
	public:
		Resource(const HashedStringView& concrete_class_name);
		virtual ~Resource() override;

		virtual bool LoadFromFile(const stdfs::path& path) { return true; }
	};
}