#pragma once
#include <Engine/Core/CoreTypes.h>
#include <Engine/Core/HandlePointer.h>

#include <memory>
#include <wrl.h>
#include <type_traits>

namespace engine
{
	// GPU 스마트포인터
	using Microsoft::WRL::ComPtr;
}