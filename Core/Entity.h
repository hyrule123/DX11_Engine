#pragma once
#include <Engine/Core/CoreMinimal.h>
#include <Engine/Core/Singleton.h>

//////////////////////////////////////////////
// Entity + EntityManager + EntityRegistrar //
//////////////////////////////////////////////

#define ENTITY_INFO(derived, base) \
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

        virtual ~Entity();

        virtual void Init() {}
        
        Entity(Entity&& move) noexcept = delete;
        Entity& operator=(const Entity& copy) = delete;
        Entity& operator=(Entity&& move) noexcept = delete;

        const HashedStringView& GetConcreteClassName() const { return concrete_class_name_; }

        uint32 GetInstanceID() const { return instance_ID_; }
    private:
        HashedStringView concrete_class_name_ = ""_hash; //컴파일 타임에 결정되는 클래스 이름

        uint32 instance_ID_ = 0;	//런타임 ID(매 실행마다 달라질 수 있음)
        inline static uint32 next_instance_ID_ = 1;
    };

    class EntityManager
    {
        DECLARE_SINGLETON(EntityManager)

    public:
        using EntityCreationFunc = u_ptr<Entity>(*)();
        template <typename T>
        void AddCtor(const HashedStringView& key)
        {
            if constexpr (std::is_default_constructible_v<T>)
            {
                if (false == key.IsEmpty())
                {
                    entity_creator_table_.insert(key, &CreateEntityUpcast<T>);
                }
            }
        }

        template <typename T>
        static u_ptr<T> CreateEntity()
        {
            static_assert(std::is_base_of_v<Entity, T>, "T must be derived from Entity");
            u_ptr<T> entity = std::make_unique<T>();
            entity->Init();
            return entity;
        }

        u_ptr<Entity> CreateEntity(const HashedStringView& key);

        template <typename T> requires std::is_base_of_v<Entity, T>
        u_ptr<T> CreateEntityAs(const HashedStringView& key)
        {
            static_assert(std::is_base_of_v<Entity, T>, "T must be derived from Entity");
            u_ptr<Entity> entity = CreateEntity(key);
            if (nullptr == entity) { return nullptr; }

            T* casted = dynamic_cast<T*>(entity.get());
            if (nullptr == casted) { return nullptr; }   // entity가 살아있으므로 여기서 정상 해제

            entity.release();          // 소유권 포기 (캐스팅 성공이 확인된 뒤에만)
            return u_ptr<T>(casted);
        }
    private:
        template <typename T>
        static u_ptr<Entity> CreateEntityUpcast()
        {
            return CreateEntity<T>();
        }

        StringHashTable<EntityCreationFunc> entity_creator_table_ = {};
    };

    template<class T>
    struct EntityRegistrar
    {
        EntityRegistrar(const HashedStringView& key)
        {
            EntityManager::GetInst().AddCtor<T>(key);
        }
    };
}

