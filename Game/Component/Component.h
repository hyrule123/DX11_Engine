#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Game/Component/ComponentCategory.h>

namespace engine
{
    class GameObject;

    class Component :
        public Entity
    {
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

    private:
        ComponentCategory category_ = {};

        w_ptr<GameObject> owner_ = {};
    };
}


