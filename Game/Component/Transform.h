#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Game/Component/ComponentCategory.h>

namespace engine
{
    class Transform final :
        public Component
    {
        COMPONENT_CATEGORY(ComponentCategory::kTransform)
    public:
        Transform();
        virtual ~Transform() override;
        
        virtual void Init() override;
    };
}


