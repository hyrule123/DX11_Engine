#include "Engine/Core/pch.h"
#include "RenderPass.h"

namespace engine
{
	RenderPass::RenderPass(const std::string_view concrete_class_name, RenderPassOrder pass_order)
		: Super(concrete_class_name)
		, pass_order_(pass_order)
	{}

	RenderPass::~RenderPass()
	{}
}

