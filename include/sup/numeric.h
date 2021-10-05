#pragma once

#include "utility.h"
#include <functional>
#include <limits>

namespace sup {
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
	requires (InitsDirectly<U, T> || requires { T{U::value}; }) {
		if constexpr (!InitsDirectly<U, T>) raw = U::value; else raw = u;
	}
	template<InitsDirectly<T> U> constexpr Num(Num<U> const& u): raw{U{u}} {}

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
	
	explicit constexpr operator T() const { return raw; }
	explicit constexpr operator bool() const { return !!raw; }
};
template<typename T> Num(T) -> Num<T>;
template<typename T> requires requires { T::value; } Num(T) ->
	Num<std::remove_cvref_t<decltype(T::value)>>;
} // namespace sup

namespace std {
template<typename T> struct numeric_limits<sup::Num<T>>: numeric_limits<T> {};
} // namespace std