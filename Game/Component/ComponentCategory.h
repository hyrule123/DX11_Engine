#pragma once

#include <Engine/Core/Type.h>

#include <type_traits>

#define COMPONENT_CATEGORY(component_category) \
public: \
constexpr static ComponentCategory kComponentCategory = component_category;

namespace engine
{
    enum class ComponentCategory : size_t
    {
        kTransform,
        kRenderer,

        kOthers,
        kEnd = kOthers
    };

    template <typename T>
    concept HasComponentCategory = 
        requires 
    { 
        {
            T::kComponentCategory
        };
    };


}