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
		Resource(const std::string_view concrete_class_name);
		virtual ~Resource() override;

		virtual bool LoadFromFile(const stdfs::path& path);

		void SetResKey(const stdfs::path& path) { res_key_ = path; }
		const stdfs::path& GetResKey() const { return res_key_; }

	private:
		stdfs::path res_key_;
	};
}