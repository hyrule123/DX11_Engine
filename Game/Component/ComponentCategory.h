#pragma once

#include <Engine/Core/CoreMinimal.h>

#include <type_traits>

#define COMPONENT_CATEGORY(component_category) \
public: \
constexpr static ComponentCategory kComponentCategory = component_category;

namespace engine
{
    enum class ComponentCategory : int32
    {
        kScripts = -1,

        // 1. 데이터 및 로직
        kBlackboard = 0,
        kBehavior,

        //여기에 Collider?
        kCollider,

        // 2. 물리 및 이동
        kTransform,

        // 3. 시각적 상태
        kAnimator,

        // 4. 화면 출력
        kCamera,
        kRenderer,
        
        kEnd
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