#pragma once
#include <Engine/Resource/Resource.h>

namespace engine 
{
    class Shader :
        public Resource
    {
    public:
        Shader(const std::string_view class_name);
        virtual ~Shader() override;

        
    };
}


