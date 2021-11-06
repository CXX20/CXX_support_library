#pragma once

#include "utility.h"
#include <functional>
#include <limits>

namespace sup {
template<typename T> concept Numeric =
	requires { typename std::numeric_limits<T>; };
template<typename T> concept Floating =
	Numeric<T> && !std::numeric_limits<T>::is_integer;
template<typename T> concept Integral = Numeric<T> && !Floating<T>;
template<typename T> concept Signed =
	Integral<T> && std::numeric_limits<T>::is_signed;
template<typename T> concept Unsigned = Integral<T> && !Signed<T>;
template<typename T> concept Boolean = Unsigned<T> && !requires(T t) { ++t; };

template<std::integral T> class Num { // TODO float, bool, T*, non-fundamental
	T raw{};

	template<typename F> constexpr auto& op_eq(Num const t, F op) {
		if constexpr (std::unsigned_integral<T> && sizeof(T) < sizeof(int))
			raw = static_cast<T>(op(unsigned{raw}, unsigned{t.raw}) %
					(std::numeric_limits<T>::max() + 1u));
		else raw = static_cast<T>(op(raw, t.raw));
		return *this;
	}

public:
	template<typename U = T> constexpr Num(U const& u = {})
	requires (Convertible<U, T> || requires { T{U::value}; }) {
		if constexpr (!Convertible<U, T>) raw = U::value; else raw = u;
	}
	template<Convertible<T> U> constexpr Num(Num<U> const& u): raw{U{u}} {}

	constexpr auto& operator+=(Num t) { return op_eq(t, std::plus{}); }
	constexpr auto& operator-=(Num t) { return op_eq(t, std::minus{}); }
	constexpr auto& operator*=(Num t) { return op_eq(t, std::multiplies{}); }
	constexpr auto& operator/=(Num t) { return op_eq(t, std::divides{}); }
	constexpr auto& operator%=(Num t) { return op_eq(t, std::modulus{}); }
	friend constexpr auto operator+(Num a, Num const b) { return a += b; }
	friend constexpr auto operator-(Num a, Num const b) { return a -= b; }
	friend constexpr auto operator*(Num a, Num const b) { return a *= b; }
	friend constexpr auto operator/(Num a, Num const b) { return a /= b; }
	friend constexpr auto operator%(Num a, Num const b) { return a %= b; }
	constexpr auto& operator++() { return ++raw, *this; }
	constexpr auto& operator--() { return --raw, *this; }
	constexpr Num operator++(int) { return raw++; }
	constexpr Num operator--(int) { return raw--; }
	constexpr Num operator-() const requires std::signed_integral<T> {
		return static_cast<T>(-raw);
	}

	constexpr auto operator<=>(Num const&) const = default;

	constexpr explicit operator T() const { return raw; }
	constexpr explicit operator bool() const { return !!raw; }
};
template<typename T> Num(T) -> Num<T>;
template<typename T> requires requires { T::value; } Num(T) ->
	Num<std::remove_cvref_t<decltype(T::value)>>;

using Size = Num<std::size_t>;
#if CHAR_BIT == 8
using I8  = Num<std::int8_t>;
using I16 = Num<std::int16_t>;
using I32 = Num<std::int32_t>;
using I64 = Num<std::int64_t>;
using U8  = Num<std::uint8_t>;
using U16 = Num<std::uint16_t>;
using U32 = Num<std::uint32_t>;
using U64 = Num<std::uint64_t>;
#endif
} // namespace sup

namespace std {
template<typename T> struct numeric_limits<sup::Num<T>>: numeric_limits<T> {};
} // namespace std