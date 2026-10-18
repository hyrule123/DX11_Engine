#pragma once
#include <Engine/Core/CoreTypes.h>

namespace engine
{
	constexpr const wchar_t* kDefaultTitleW = L"DX11_Engine";
	constexpr const wchar_t* kDefaultClassNameW = L"DX11_Engine_Class";

	constexpr uint32_t kDefaultWidth = 1280;
	constexpr uint32_t kDefaultHeight = 960;

	constexpr float kDeltaTimeCap = 0.1f;
	constexpr float kDefaultFixedDeltaTime = 1.0f / 60.0f;
	constexpr uint32_t kDefaultMaxStepCount = 3u;

	constexpr uint32_t kMaxRenderTargetCount = 8u;

	constexpr float kDefaultBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr uint32_t kDefaultSampleMask = 0xFF'FF'FF'FFu;

	constexpr uint32_t kMaxLayers = 32u;

	constexpr uint32_t kMaxDrainLoopCount = 8u;

	constexpr int32_t kInvalidIndex = -1;

	constexpr uint32_t kInvalid32 = std::numeric_limits<uint32_t>::max();
	constexpr uint64_t kInvalid64 = std::numeric_limits<uint64_t>::max();

	constexpr size_t kInvalidSize = std::numeric_limits<size_t>::max();
}