#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Core/CoreMinimal.h>

// ComputeShader: 코드(처리 방법)만 == Resource
// ComputePass: ComputeShader + 데이터 준비 + 결과 저장
// 스레드 그룹 계산, Bind - Dispatch - Unbind, UAV/SRV 바인딩 등등
namespace engine
{
    class ComputeShader;
    class ComputePass :
        public Entity
    {
		ENTITY_INFO(ComputePass, Entity)
    public:
		ComputePass(const HashedStringView& concrete_class_name);
		virtual ~ComputePass() override;

        // Init에서 호출하세요
		bool LoadComputeShader(const HashedStringView& res_key);

		void Execute(ID3D11DeviceContext* context);

    protected:
        virtual void BindResources(ID3D11DeviceContext* context) = 0;
        virtual void UnbindResources(ID3D11DeviceContext* context) = 0;

        // 그룹 수 아님, 스레드 총 개수
		virtual std::array<UINT, 3> GetThreadCount() const = 0;

    private:
		s_ptr<ComputeShader> compute_shader_ = nullptr;
    };
}

