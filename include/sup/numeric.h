#pragma once

#include "type_traits.h"
#include <limits>
#include <utility>

namespace sup {
template<typename T> class Num {
	T raw{};

public:
	template<typename U = T> constexpr Num(U const u = {})
	requires (InitsDirectly<U, T> || requires { T{U::value}; }) {
		if constexpr (InitsDirectly<U, T>) raw = u; else raw = U::value;
	}
	template<InitsDirectly<T> U> constexpr Num(Num<U> const& u): raw{U{u}} {}

	constexpr auto& operator+=(Num const t) {
		if constexpr (std::unsigned_integral<T> && sizeof(T) < sizeof(int))
			raw = static_cast<T>(unsigned{raw} + unsigned{t.raw} %
					(std::numeric_limits<T>::max() + 1u));
		else raw += t.raw;
		return *this;
	}
	constexpr auto operator+(Num const t) const { return Num{*this} += t; }
	constexpr auto operator+(std::constructible_from<Num> auto u) const {
		return decltype(u){*this} += u;
	}
	constexpr auto& operator++() { return *this += value_v<1>; }
	constexpr Num operator++(int) { return raw++; }

	constexpr auto operator<=>(Num const&) const = default;

	explicit constexpr operator T() const { return raw; }
};
template<typename T> Num(T) -> Num<T>;
} // namespace sup