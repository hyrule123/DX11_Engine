#pragma once

#include <Engine/Core/Windows.h>

//DirectX
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

#include <cassert>

constexpr UINT GetFormatByteSize(DXGI_FORMAT f)
{
    switch (f)
    {
    case DXGI_FORMAT_R8_UINT:            return 1;
    case DXGI_FORMAT_R16_UINT:           return 2;
    case DXGI_FORMAT_R32_UINT:           return 4;
    case DXGI_FORMAT_R8G8B8A8_UNORM:     return 4;
    default: assert(false); return 0;
    }
}