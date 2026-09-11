#pragma once
#include <Engine/Core/CoreMinimal.h>

#include <Engine/Render/RenderTypes.h>

#include <span>

namespace engine::culling
{
    void CullRenderers2D(const AABB2D& view_bounds, std::span<RendererInfo2D> renderer_infos, std::vector<uint32>& out_visible);
}