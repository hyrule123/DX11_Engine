#pragma once
#include <Engine/Core/StdType.h>
#include <Engine/Core/HandlePointer.h>

#include <memory>
#include <wrl.h>
#include <type_traits>

namespace engine
{
	/* Smart pointer types */
	template <typename T>
	using u_ptr = std::unique_ptr<T>;
	template <typename T>
	using s_ptr = std::shared_ptr<T>;
	template <typename T>
	using w_ptr = std::weak_ptr<T>;

	// GPU 스마트포인터
	using Microsoft::WRL::ComPtr;

	// 자체 제작 weak handle pointer
	template <typename T>
	using wh_ptr = weak_handle_ptr<T>;
}