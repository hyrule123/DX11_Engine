#pragma once

#include <memory>
#include <wrl.h>

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
}