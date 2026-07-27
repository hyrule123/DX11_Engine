#pragma once
#include <Engine/Resource/Resource.h>

#include <Engine/Core/UtilMacro.h>

#include <Engine/Core/DX11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace engine 
{
    class Shader :
        public Resource
    {
        CLASS_INFO(Shader, Resource)
    public:
        Shader(const HashedStringView& concrete_class_name);
        virtual ~Shader() override;

        virtual bool LoadFromFile(const stdfs::path& path) final;
        virtual void Bind(ID3D11DeviceContext* context) = 0;
    protected:
        virtual bool Create( const std::vector<uint8>& bytecode) = 0;
    };
}


