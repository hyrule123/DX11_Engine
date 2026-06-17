#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/GameObject.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/UtilMacro.h>

namespace engine
{
    class Component :
        public Entity
    {
        CLASS_INFO(Component, Entity)
    public:
        Component(
            const std::string_view concrete_class_name, 
            ComponentCategory category
            );
        virtual ~Component() override;

        virtual void Init() = 0;
        virtual void Awake();
        virtual void Start();
        
        virtual void Update() {}
        virtual void LateUpdate() {}
        virtual void Render() {}

        ComponentCategory GetComponentCategory() const { return category_; }

        void SetOwner(s_ptr<GameObject> owner) { owner_ = owner; }
        s_ptr<GameObject> GetOwner() const { return owner_.lock(); }
        s_ptr<GameObject> GetGameObject() const { return GetOwner(); }
        
        template <typename T>
        s_ptr<T> GetComponent() {
            if (!(owner_.expired())) {
                return owner_.lock()->GetComponent<T>();
            }
            return nullptr;
        }

        bool HasInitialzed() const { return has_initialized_; }
        bool HasAwaken() const { return has_awaken_; }
        bool HasStarted() const { return has_started_; }

    private:
        ComponentCategory category_ = {};

        w_ptr<GameObject> owner_ = {};

        bool has_initialized_ = false;
        bool has_awaken_ = false;
        bool has_started_ = false;
    };
}


