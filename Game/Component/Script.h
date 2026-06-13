#pragma once
#include <Engine/Game/Component/Component.h>

#include <Engine/Game/Component/ComponentCategory.h>

namespace engine
{
    class Script :
        public Component
    {
        COMPONENT_CATEGORY(ComponentCategory::kOthers)
    public:
        Script();
        virtual ~Script() override;

        virtual void Init() override {}
    };
}


