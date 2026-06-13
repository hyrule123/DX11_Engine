#pragma once

#include <Engine/Core/Common.h>
#include <Engine/Core/ClassMacro.h>

namespace engine
{
	//모든 오브젝트의 루트 클래스
	//Resource, GameObject, Component 등 모든 클래스는 Entity를 상속받는다.
	class Entity
		: public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(const std::string_view concrete_class_name);
		virtual ~Entity();

		const std::string_view GetConcreteClassName() const { return concrete_class_name_; }

		void SetName(const std::string_view name) { name_ = name; }
		const std::string& GetName() const { return name_; }

	private:
		std::string_view concrete_class_name_ = {};
		uint64 id_ = {};
		static inline uint64 next_id_ = 0;
		std::string name_ = {};
	};
}

