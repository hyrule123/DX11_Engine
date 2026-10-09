#pragma once
#include <Engine/Core/Entity.h>

#include <Engine/Render/RenderTypes.h>

struct ID3D11DeviceContext;

namespace engine
{
    // 모든 패스의 공통 베이스.
    // - ID는 슬롯 식별자일 뿐 실행 순서가 아니다(실행 순서는 RenderManager의 exec_order_).
    // - 출력 대상은 패스가 갖지 않는다. 기본은 ctx.view의 대상이며,
    //   대상이 고정된 패스(Present, 그림자 등)만 파생 클래스에서 직접 보유한다.
    class RenderPass
        : public Entity
    {
        ENTITY_INFO(RenderPass, Entity)
    public:
        explicit RenderPass(const HashedStringView& concrete_class_name, render_pass::ID id);
        virtual ~RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        virtual void Execute(const RenderContext& ctx) = 0;

        render_pass::ID GetID() const { return id_; }

    private:
        render_pass::ID id_;
    };
}