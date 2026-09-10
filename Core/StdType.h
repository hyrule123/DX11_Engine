#pragma once
#define NOMINMAX
#include <cstdint>
#include <unordered_map>
#include <string>

namespace engine
{
	/* Standard integer types */
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

	struct int32_2 { int32 x, y; };
	struct uint32_2 { uint32 x, y; };

	union RenderKey
	{
		struct
		{
			uint32 material_id;
			uint32 mesh_id;
		};
		uint64 key = {};

		auto operator<=>(const RenderKey& other) const {
			return key <=> other.key;
		}
		auto operator==(const RenderKey& other) const {
			return key == other.key;
		}
	};

	struct RenderKeyHasher
	{
		using is_transparent = void;
		size_t operator()(const RenderKey& key) const noexcept
		{
			return std::hash<uint64>{}(key.key);
		}
	};

	struct DataBlock
	{
		uint8* ptr = {};
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

	enum class RegisterType : uint8 { T, B, S, U };

	template <RegisterType Type>
	struct RegisterSlot
	{
		uint32 slot_num;
		constexpr explicit RegisterSlot(uint32 slot_num) : slot_num(slot_num) {}
		constexpr uint32 Get() const { return slot_num; }

		friend constexpr bool operator==(RegisterSlot a, RegisterSlot b)
		{
			return a.slot_num == b.slot_num;
		}
	};

	using RegisterT = RegisterSlot<RegisterType::T>;
	using RegisterB = RegisterSlot<RegisterType::B>;
	using RegisterS = RegisterSlot<RegisterType::S>;
	using RegisterU = RegisterSlot<RegisterType::U>;


}