#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/UtilMacro.h>

namespace engine 
{
    class Shader :
        public Resource
    {
        CLASS_INFO(Shader, Resource)
    public:
        Shader(const std::string_view concrete_class_name);
        virtual ~Shader() override;

        virtual bool LoadFromFile(const stdfs::path& path) final;
        virtual void Bind(ID3D11DeviceContext* context) = 0;
    protected:
        virtual bool Create(const ComPtr<ID3D11Device>& device, const std::vector<uint8>& bytecode) = 0;
    };
}


