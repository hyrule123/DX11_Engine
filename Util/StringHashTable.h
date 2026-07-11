#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

static constexpr size_t hash_str_fnv1a(const std::string_view s) {
	size_t hash = 14695981039346656037ULL;
	for (char c : s) {
		hash ^= static_cast<size_t>(c);
		hash *= 1099511628211ULL;
	}
	return hash;
}

// HashedStringView 구조체, constexpr
class HashedStringView {
public:
	constexpr HashedStringView(const std::string_view s) : hash(hash_str_fnv1a(s)), str_view(s) {}

	size_t GetHash() const { return hash; }
	std::string_view GetStringView() const { return str_view; }

private:
	//HashedString용 생성자, 해시값을 직접 넣어주는 용도
	friend class HashedString;
	explicit HashedStringView(const std::string_view s, size_t h) : hash(h), str_view(s) {}
	HashedStringView() = delete;

	size_t hash;
	std::string_view str_view;
};

// ""_hash 리터럴 연산자, consteval
consteval HashedStringView operator "" _hash(const char* s, size_t len) {
	return HashedStringView(std::string_view(s, len));
}

// HashedString 구조체, 해시값과 문자열을 함께 저장(런타임용)
class HashedString {
public:
	HashedString(const std::string_view s) : str(s), value(str) {}
	HashedString(const std::string& s) : str(s), value(str) {}
	explicit HashedString(std::string&& s) : str(std::move(s)), value(str) {}

	//문자열은 복사, 해시값은 그대로 사용
	HashedString(const HashedString& s) : str(s.str), value(str, s.value.hash) {}

	//Hashed String View가 들어오면 문자열만 복사하고 해시값은 그대로 사용
	HashedString(const HashedStringView& sv) : str(sv.str_view), value(str, sv.hash) {}

	HashedString& operator =(const HashedStringView& s) {
		HashedString temp(s);

		str.swap(temp.str);
		value.str_view.swap(temp.value.str_view);
		value.hash = temp.value.hash;

		return *this;
	}

	HashedString() = delete;

	// 내부의 HashedStringView를 반환하는 연산자
	operator const HashedStringView& () const { return value; }

	const HashedStringView& GetHashedStringView() const { return value; }
	const std::string& GetString() const { return str; }

private:
	std::string str;
	HashedStringView value;
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
		return s.GetHash();
	}
};

struct StringEqual {
	using is_transparent = void;

	bool operator()(const std::string_view lhs, const std::string_view rhs) const { return lhs == rhs; }

	bool operator()(const std::string_view lhs, const HashedStringView& rhs) const { return lhs == rhs.GetStringView(); }
	bool operator()(const HashedStringView& lhs, const std::string_view rhs) const { return lhs.GetStringView() == rhs; }

	bool operator()(const HashedStringView& lhs, const HashedStringView& rhs) const {
		return lhs.GetHash() == rhs.GetHash() && lhs.GetStringView() == rhs.GetStringView();
	}
};

template <typename T>
using StringHashTable = std::unordered_map<std::string, T, StringHasher, StringEqual>;