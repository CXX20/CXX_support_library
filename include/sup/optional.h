#pragma once

#include "numeric.h"

namespace sup {
/// Allowed to be specialized for client-defined types.
template<typename T> class Opt;
template<typename T> class Opt<T&> {
	T* raw{};
public:
	constexpr Opt() {}
	constexpr Opt(T& src): raw{std::addressof(src)} {}
	constexpr Opt(T&&) = delete;
	constexpr T const& operator*() const { return *raw; }
	constexpr T const* operator->() const { return raw; }
	constexpr T& operator*() { return *raw; }
	constexpr T* operator->() { return raw; }
	constexpr explicit operator bool() const { return raw != nullptr; }
};
template<Floating T> class Opt<T> {
	static_assert(std::numeric_limits<T>::has_signaling_NaN);
	static auto constexpr null = std::numeric_limits<T>::signaling_NaN();
	T raw{null};
public:
	constexpr Opt() {}
	constexpr Opt(T const src): raw{src} { assert(*this); }
	constexpr T const& operator*() const { return assert(*this), raw; }
	constexpr T& operator*() { return assert(*this), raw; }
	constexpr explicit operator bool() const {
		using Memcmp = std::array<unsigned char, sizeof(T)>;
		return std::bit_cast<Memcmp>(raw) != std::bit_cast<Memcmp>(null);
	}
};
template<Integral T> class Opt<T> {
	static auto constexpr null = Signed<T> ?
		std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
	T raw{null};
public:
	constexpr Opt() {}
	constexpr Opt(T const src): raw{src} { assert(*this); }
	constexpr T const& operator*() const { return assert(*this), raw; }
	constexpr T& operator*() { return assert(*this), raw; }
	constexpr explicit operator bool() const { return raw != null; }
};
template<Boolean T> class Opt<T> {
	static char constexpr null{2};
	class Fake {
		friend class Opt;
		char& real;
		constexpr Fake(char& src): real{src} {}
	public:
		constexpr Fake& operator=(T const b) { return (void)(real = b), *this; }
		constexpr operator T() const { return real != 0; }
	};
	char raw{null};
public:
	constexpr Opt() {}
	constexpr Opt(T const src): raw{src} {}
	constexpr T operator*() const { return assert(*this), raw != 0; }
	constexpr Fake operator*() { return assert(*this), raw; }
	constexpr explicit operator T() const { return raw != null; }
};
} // namespace sup