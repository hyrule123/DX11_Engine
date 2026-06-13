#pragma once

#include <Engine/Core/Common.h>
#include <Engine/Core/Entity.h>

namespace engine
{
	class Resource : public Entity
	{
	public:
		Resource(const std::string_view concrete_class_name);
		virtual ~Resource() override;

		virtual bool LoadFromFile(const stdfs::path& path);

		inline void SetPath(const stdfs::path& path) { res_path_ = path; }
		inline const stdfs::path& GetPath() const { return res_path_; }

	private:
		stdfs::path res_path_;
	};
}