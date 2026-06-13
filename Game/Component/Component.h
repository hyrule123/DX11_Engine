#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/Component/ComponentCategory.h>

#include <Engine/Core/ClassMacro.h>

namespace engine
{
    class GameObject;

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

        ComponentCategory GetComponentCategory() const { return category_; }

        void SetOwner(s_ptr<GameObject> owner) { owner_ = owner; }
        w_ptr<GameObject> GetOwner() const { return owner_; }

        bool HasInitialzed() const { return has_initialized_; }

    private:
        ComponentCategory category_ = {};

        w_ptr<GameObject> owner_ = {};

        bool has_initialized_ = false;
    };
}


