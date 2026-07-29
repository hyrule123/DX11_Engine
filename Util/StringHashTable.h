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
	template <size_t N>
	consteval HashedStringView(const char(&s)[N]) :
		str_view(s, N - 1),
		hash(hash_str_fnv1a(str_view))
	{}

	constexpr HashedStringView(const std::string_view s) : 
		str_view(s), 
		hash(hash_str_fnv1a(str_view)) 
	{}
	HashedStringView() : HashedStringView(""_hash) {}

	bool IsEmpty() const { return str_view.empty(); }
	size_t GetHash() const { return hash; }
	std::string_view GetStringView() const { return str_view; }

	bool operator==(const HashedStringView& other) const {
		return (hash == other.hash) && (str_view == other.str_view);
	}
	

private:
	friend class HashedString;
	HashedStringView(const std::string& s, const size_t h) : str_view(s), hash(h) {}

	std::string_view str_view;
	size_t hash;
};

// ""_hash 리터럴 연산자, consteval
consteval HashedStringView operator "" _hash(const char* s, size_t len) {
	return HashedStringView(std::string_view(s, len));
}

// HashedString 구조체, 해시값과 문자열을 함께 저장(런타임용)
class HashedString {
public:
	HashedString(const std::string_view s) : str(s), hash(hash_str_fnv1a(s)) {}
	HashedString(const std::string& s) : str(s), hash(hash_str_fnv1a(s)) {}
	HashedString(std::string&& s) : str(std::move(s)), hash(hash_str_fnv1a(str)) {}

	HashedString(HashedString&& move) noexcept : str(std::move(move.str)), hash(move.hash) {}
	HashedString& operator=(HashedString&& move) noexcept {
		str = std::move(move.str);
		hash = move.hash;
		return *this;
	}

	//문자열은 복사, 해시값은 그대로 사용
	HashedString(const HashedString& s) : 
		str(s.str), 
		hash(s.hash)
	{}
	HashedString& operator=(const HashedString& s) {
		str = s.str;
		hash = s.hash;
		return *this;
	}

	//Hashed String View가 들어오면 문자열만 복사하고 해시값은 그대로 사용
	HashedString(const HashedStringView& sv) : 
		str(sv.GetStringView()), 
		hash(sv.GetHash()) 
	{}
	HashedString& operator=(const HashedStringView& sv) {
		str = std::string(sv.GetStringView());
		hash = sv.GetHash();
		return *this;
	}
	HashedString() : HashedString(""_hash) {}

	HashedStringView GetHashedStringView() const { return HashedStringView(str, hash); }
	// HashedStringView를 반환하는 연산자
	operator HashedStringView () const { return GetHashedStringView(); }

	const std::string& GetString() const { return str; }

private:
	std::string str;
	size_t hash;
};

struct StringHasher {
	using is_transparent = void;

	size_t operator()(const std::string_view s) const noexcept {
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
};

//혹시나 내부기능이 필요하면 cont에 직접 접근해서 쓰면됨
template <typename ValueType>
struct StringHashTable
{
	using ContType = std::unordered_map<std::string, ValueType, StringHasher, StringEqual>;
	using Iterator = ContType::iterator;
	using ConstIterator = ContType::const_iterator;

	ValueType& operator[](const HashedStringView& key) {
		auto it = cont.find(key);
		if (it != cont.end()) {
			return it->second;
		}

		// 못 찾았을 때만 key.str(string_view)를 이용해 런타임에 메모리 할당 후 삽입
		return cont[std::string(key.GetStringView())];
	}

	ConstIterator find(const HashedStringView& key) const {
		 return cont.find(key);
	}

	Iterator find(const HashedStringView& key) {
		return cont.find(key);
	}

	Iterator end() noexcept {
		return cont.end();
	}
	ConstIterator end() const noexcept {
		return cont.end();
	}


	bool insert(const HashedStringView& key, const ValueType& value) {
		auto [it, inserted] =
			cont.emplace(std::string(key.GetStringView()), value);
		return inserted;
	}

	bool insert(const HashedStringView& key, ValueType&& value) {
		auto [it, inserted] =
			cont.emplace(std::string(key.GetStringView()), std::move(value));
		return inserted;
	}

	bool erase(const HashedStringView& key) noexcept {
		return erase(cont.find(key));
	}
	bool erase(Iterator it) noexcept {
		if (it != end()) {
			cont.erase(it);
			return true;
		}
		return false;
	}

	std::unordered_map<std::string, ValueType, StringHasher, StringEqual> cont = {};
};