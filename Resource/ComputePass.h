#pragma once
#include <Engine/Core/Entity.h>

namespace engine
{
    class ComputePass :
        public Entity
    {
		ENTITY_INFO(ComputePass, Entity)
    public:
		ComputePass();
		virtual  ~ComputePass() override;


    };
}

