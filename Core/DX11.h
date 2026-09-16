#pragma once
#include <Engine/Core/Windows.h>
#include <Engine/Core/StdType.h>
#include <Engine/Core/Enum.h>
#include <Engine/Core/SmartPointer.h>

#include <Engine/Render/RenderTypes.h>

#include <cassert>

//DirectX
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")

namespace engine
{
	static_assert(sizeof(UINT) == sizeof(uint32), "UINT과 uint32_t의 크기가 다릅니다. 사이즈를 일치시키세요.");

	constexpr UINT GetDXGIFormatByteStride(DXGI_FORMAT f)
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

	class ConstantBuffer;
	class ShaderResource;
	struct ConstantBufferBindingInfo
	{
		ShaderStageFlags stage_flag = {};
		uint32 slot = 0u;
		s_ptr<ConstantBuffer> constant_buffer = {};
	};
	struct ShaderResourceBindingInfo
	{
		ShaderStageFlags stage_flag = {};
		uint32 slot = 0u;
		s_ptr<ShaderResource> shader_resource = {};
	};
}

