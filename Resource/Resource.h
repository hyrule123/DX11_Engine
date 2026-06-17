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

		void SetPath(const stdfs::path& path) { res_path_ = path; }
		const stdfs::path& GetPath() const { return res_path_; }

	private:
		stdfs::path res_path_;
	};
}