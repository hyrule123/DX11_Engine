#pragma once

#include <Engine/Common.h>
#include <Engine/Entity.h>

namespace engine
{
	class Resource : public Entity
	{
	public:
		Resource(const std::string_view class_name);
		virtual ~Resource() override;

		virtual bool LoadFromFile(const stdfs::path& path);

		inline const stdfs::path& GetPath() const { return res_path_; }

	private:
		stdfs::path res_path_;
	};
}