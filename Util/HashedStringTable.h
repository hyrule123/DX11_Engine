#pragma once

#include <string>
#include <string_view>

static constexpr size_t hash_str_fnv1a(const std::string_view s) {
	size_t hash = 14695981039346656037ULL;
	for (char c : s) {
		hash ^= static_cast<size_t>(c);
		hash *= 1099511628211ULL;
	}
	return hash;
}

// HashedStringView 구조체, constexpr
struct HashedStringView {
public:
	const size_t hash;
	const std::string_view str_view;
	constexpr HashedStringView(const std::string_view s) : hash(hash_str_fnv1a(s)), str_view(s) {}

private:


	//HashedString용 생성자, 해시값을 직접 넣어주는 용도
	friend struct HashedString;
	explicit HashedStringView(const std::string_view s, size_t h) : hash(h), str_view(s) {}
	HashedStringView() = delete;
};

// ""_hash 리터럴 연산자, consteval
consteval HashedStringView operator "" _hash(const char* s, size_t) {
	return HashedStringView(s);
}

// HashedString 구조체, 해시값과 문자열을 함께 저장(런타임용)
struct HashedString {
	HashedString(const std::string_view s) : str(s), value(str) {}

	//Hashed String View가 들어오면 문자열만 복사하고 해시값은 그대로 사용
	HashedString(const HashedStringView& sv) : str(sv.str_view), value(str, sv.hash) {}
	HashedString() = delete;

	// 내부의 HashedStringView를 반환하는 연산자
	operator const HashedStringView& () const { return value; }

	const std::string str;
	const HashedStringView value;
};

struct StringHasher {
	using is_transparent = void;

	size_t operator()(const std::string_view s) const noexcept {
		return hash_str_fnv1a(s);
	}

	size_t operator()(const std::string& s) const noexcept {
		return hash_str_fnv1a(s);
	}

	size_t operator()(const HashedStringView& s) const noexcept {
		return s.hash;
	}
};

struct StringEqual {
	using is_transparent = void;

	bool operator()(const std::string_view lhs, const std::string_view rhs) const { return lhs == rhs; }

	bool operator()(const std::string_view lhs, const HashedStringView& rhs) const { return lhs == rhs.str_view; }
	bool operator()(const HashedStringView& lhs, const std::string_view rhs) const { return lhs.str_view == rhs; }

	bool operator()(const HashedStringView& lhs, const HashedStringView& rhs) const {
		return lhs.hash == rhs.hash && lhs.str_view == rhs.str_view;
	}
};

template <typename T>
using HashedStringTable = std::unordered_map<std::string, T, StringHasher, StringEqual>;