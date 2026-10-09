#include "Engine/Core/pch.h"
#include "RenderPass.h"

namespace engine
{
    RenderPass::RenderPass(const HashedStringView& concrete_class_name, render_pass::ID id)
        : Entity(concrete_class_name), id_(id)
    {}

    RenderPass::~RenderPass() = default;
}