#pragma once
#include <Engine/Game/Component/Component.h>
#include <Engine/Game/Component/ComponentCategory.h>

namespace engine
{
    class Script :
        public Component
    {
        CLASS_INFO(Script, Component)
            COMPONENT_CATEGORY(ComponentCategory::kOthers)
    public:
        Script(const std::string_view class_name);
        virtual ~Script() override;

        virtual void Init() override {}

        template <typename T>
        s_ptr<T> AddComponent() {
            return GetOwner()->AddComponent<T>();
        }
    };
}