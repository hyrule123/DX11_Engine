#pragma once
#define NOMINMAX
#include <cstdint>
#include <stdint.h>
#include <unordered_map>
#include <string>

namespace engine
{
	struct int32_2 { int32_t x, y; };
	struct uint32_2 { uint32_t x, y; };

	struct DataBlock
	{
		uint8_t* ptr = {};
		size_t size = {};

		bool IsValid() const { return (ptr != nullptr && size > 0); }
		template <typename T>
		void Write(const T& value) const
		{
			static_assert(std::is_trivially_copyable_v<T>);
			if (ptr && sizeof(T) <= size) { memcpy(ptr, &value, sizeof(T)); }
		}
	};

	// 스코프 내에서 특정 값의 임시 변경을 보장하는 유틸리티 클래스(가드)
	template <typename T>
		requires std::is_nothrow_move_constructible_v<T>&&
	std::is_nothrow_move_assignable_v<T>
		class [[nodiscard]] ScopedValue
	{
	public:
		explicit ScopedValue(T& target, T value) noexcept
			: target_(target)
			, previous_(std::exchange(target_, std::move(value)))
		{}

		~ScopedValue() noexcept
		{
			target_ = std::move(previous_);
		}

		ScopedValue(const ScopedValue&) = delete;
		ScopedValue& operator=(const ScopedValue&) = delete;

		ScopedValue(ScopedValue&&) = delete;
		ScopedValue& operator=(ScopedValue&&) = delete;

	private:
		T& target_;
		T previous_;
	};

	template <typename T>
	ScopedValue(T&, T) -> ScopedValue<T>;


	// 함수 호출(void)에 scope가 필요할 때
	template<typename Begin, typename End>
	class [[nodiscard]] ScopedCall
	{
	public:
		ScopedCall(Begin begin, End end)
			: end_(std::move(end))
		{
			static_assert(std::is_void_v<std::invoke_result_t<Begin&>>,
				"ScopedCall: begin must return void");
			static_assert(std::is_void_v<std::invoke_result_t<End&>>,
				"ScopedCall: end must return void");

			begin();
		}

		~ScopedCall()
		{
			end_();
		}

		ScopedCall(const ScopedCall&) = delete;
		ScopedCall& operator=(const ScopedCall&) = delete;

	private:
		End end_;
	};


}