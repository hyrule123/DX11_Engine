#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <Engine/Core/Singleton.h>

#include <functional>

#define CLASS_INFO(derived, base) \
private: \
using Super = base; \
public: \
constexpr static HashedStringView kClassConcreteName = STRINGIFY(derived)##_hash; \
inline static EntityRegistrar<derived> kRegistrar{kClassConcreteName};

namespace engine
{
	//모든 오브젝트의 루트 클래스
	//Resource, GameObject, Component 등 모든 클래스는 Entity를 상속받는다.
	class Entity
		: public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(const HashedStringView& concrete_class_name);
		Entity(const Entity& copy);
		Entity(const Entity&& move) noexcept;
		virtual ~Entity();

		const HashedStringView& GetConcreteClassName() const { return concrete_class_name_; }

		uint32 GetInstanceID() const { return instance_ID_; }

	private:
		HashedStringView concrete_class_name_ = ""_hash; //컴파일 타임에 결정되는 클래스 이름

		uint32 instance_ID_ = {};	//런타임 ID(매 실행마다 달라질 수 있음)
		static inline uint32 next_instance_ID_ = 1;
	};

	class EntityFactory
	{
		DECLARE_SINGLETON(EntityFactory)
	public:
		template <typename T>
		void AddCtor(const HashedStringView& key)
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				if (!key.IsEmpty())
				{
					entity_creator_table_.insert(key, 
						[]() -> s_ptr<Entity> 
						{ 
							return std::make_shared<T>(); 
						}
					);
				}
			}
		}

		s_ptr<Entity> CreateEntity(const HashedStringView& key)
		{
			auto it = entity_creator_table_.find(key);
			if (it != entity_creator_table_.end())
			{
				return it->second();
			}
			return nullptr;
		}

		template <typename T> requires std::is_base_of_v<Entity, T>
		s_ptr<T> CreateEntityAs(const HashedStringView& key)
		{
			s_ptr<Entity> entity = CreateEntity(key);
			if (entity)
			{
				return std::dynamic_pointer_cast<T>(entity);
			}
			return nullptr;
		}

	private:
		StringHashTable<std::function<s_ptr<Entity>()>> entity_creator_table_;
	};

	template<class T>
	struct EntityRegistrar
	{
		EntityRegistrar(const HashedStringView& key)
		{
			EntityFactory::GetInst().AddCtor<T>(key);
		}
	};
	
}

