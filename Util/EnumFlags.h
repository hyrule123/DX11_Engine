#pragma once
#include <Engine/Core/Debug.h>

#include <bit>
#include <type_traits>
#include <limits>

// 플래그용 enum class 요구사항:
//   1. 열거자는 '비트 인덱스'다 (0, 1, 2, ...). 이미 비트화된 값(0x01, 0x02)은 사용 불가.
//   2. 명시적 값을 대입하지 않는다. (kVS = 10 같은 형태는 비트 낭비 또는 컴파일 에러)
//   3. 마지막 열거자로 kCount를 둔다. 순회와 유효 비트 마스크 계산에 사용된다.
//   4. 인하우스 enum 전용. 외부 API의 비트 플래그에는 사용하지 않는다.


//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ 아래 매크로를 사용할것 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// enum이 선언된 네임스페이스 안에서 호출해야 한다 (ADL 요구사항).
#define ENABLE_ENUM_FLAGS(AliasName, EnumT, StorageT)                      \
	using AliasName = EnumFlags<EnumT, StorageT>;                \
	[[nodiscard]] inline constexpr AliasName operator|(EnumT a, EnumT b)   \
	{                                                                      \
		return AliasName(a) | AliasName(b);                                \
	}


template <typename EnumT, typename StorageT>
class EnumFlags
{
	static_assert(std::is_enum_v<EnumT>);
	static_assert(std::is_unsigned_v<StorageT>, "저장 타입은 부호 없는 정수여야 합니다.");

	static constexpr size_t kBitCount = std::numeric_limits<StorageT>::digits;

	static_assert((size_t)EnumT::kCount >= 1, "열거자가 최소 1개는 필요합니다.");
	static_assert((size_t)EnumT::kCount <= kBitCount,
		"열거자 개수가 저장 타입 비트 폭을 초과했습니다.");

	static constexpr StorageT kValidMask = (StorageT)(
		std::numeric_limits<StorageT>::max() >> (kBitCount - (size_t)EnumT::kCount));

public:
	constexpr EnumFlags() = default;
	constexpr EnumFlags(EnumT e) : bits_(ToBit(e)) {}

	// f = kVS 로 전체를 덮어쓰는 사고 방지. |= 를 쓰거나 EnumFlags(kVS) 를 명시할 것. (일단 주석 처리)
	// constexpr EnumFlags& operator=(EnumT) = delete;

	constexpr static EnumFlags Full() { return EnumFlags(kValidMask); }

	constexpr bool Test(EnumT e) const { return (bits_ & ToBit(e)) != 0; }

	constexpr void Set(EnumT e) { bits_ |= ToBit(e); }
	constexpr void Set() { bits_ = kValidMask; }

	constexpr void Reset(EnumT e) { bits_ &= (StorageT)~ToBit(e); }
	constexpr void Reset() { bits_ = (StorageT)0; }

	constexpr void Flip(EnumT e) { bits_ ^= ToBit(e); }
	constexpr void Flip() { bits_ ^= kValidMask; }

	constexpr bool All()  const { return bits_ == kValidMask; }
	constexpr bool Any()  const { return bits_ != 0; }
	constexpr bool None() const { return bits_ == 0; }

	constexpr size_t Count() const { return (size_t)std::popcount(bits_); }

	constexpr EnumFlags& operator|=(EnumFlags o) { bits_ |= o.bits_; return *this; }
	constexpr EnumFlags& operator&=(EnumFlags o) { bits_ &= o.bits_; return *this; }

	constexpr EnumFlags operator|(EnumFlags o) const { return EnumFlags(StorageT(bits_ | o.bits_)); }
	constexpr EnumFlags operator&(EnumFlags o) const { return EnumFlags(StorageT(bits_ & o.bits_)); }
	constexpr EnumFlags operator~() const { return EnumFlags(StorageT(~bits_ & kValidMask)); }

	constexpr bool operator==(const EnumFlags&) const = default;

private:
	// kBitCount: StorageT의 값 비트 수.
	// kValidMask: 유효 비트만 1.
	//   ~StorageT{0} 은 uint8_t/uint16_t 에서 정수 승격으로 int(-1) 이 되어
	//   우시프트 시 부호 확장되므로 사용하지 않는다. max() 는 항상 양수.

	static constexpr StorageT ToBit(EnumT e)
	{
		if ((size_t)e >= (size_t)EnumT::kCount)
		{
			ASSERT_F(false, "유효 범위를 벗어난 열거자입니다.");
			return 0;
		}
		return (StorageT)(StorageT{ 1 } << (unsigned)e);
	}

	constexpr explicit EnumFlags(StorageT raw) : bits_(raw) {}

	StorageT bits_ = 0;
};