#pragma once

#include <Engine/Core/StdType.h>

namespace engine
{
	constexpr const wchar_t* kDefaultTitleW = L"DX11_Engine";
	constexpr const wchar_t* kDefaultClassNameW = L"DX11_Engine_Class";

	constexpr uint32 kDefaultWidth = 1280;
	constexpr uint32 kDefaultHeight = 960;

	constexpr float kDeltaTimeCap = 0.1f;
	constexpr float kDefaultFixedDeltaTime = 1.0f / 60.0f;
	constexpr uint32 kDefaultMaxStepCount = 3u;

	constexpr uint32 kMaxTextureCount = 8u;
	constexpr uint32 kMaxRenderTargetCount = 8u;

	constexpr float kDefaultBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr uint32 kDefaultSampleMask = 0xFF'FF'FF'FFu;
}