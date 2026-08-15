#pragma once
#include <Engine/Resource/Graphics/Shader/Shader.h>

#include <Engine/Core/CoreMinimal.h>

#include <array>

namespace engine
{
    class ComputeShader :
        public Shader
    {
		ENTITY_INFO(ComputeShader, Shader)
    public:
        ComputeShader();
		virtual ~ComputeShader() override;

		virtual bool Create(const std::vector<uint8>& bytecode) override;

        virtual void Bind(ID3D11DeviceContext* context) override;
        void Unbind(ID3D11DeviceContext* context);

    private:
        ComPtr<ID3D11ComputeShader> compute_shader_ = {};
		std::array<UINT, 3> thread_group_size_ = {};
    };
}


