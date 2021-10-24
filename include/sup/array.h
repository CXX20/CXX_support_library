#pragma once

#include "utility.h"
#include <cassert>
#include <ranges>

namespace sup {
template<typename T, std::size_t n> class Arr {
	std::conditional_t<n != 0, T[n], T*> raw;
	template<auto... is> constexpr Arr(std::index_sequence<is...>, auto iter)
	: raw{((void)is, *iter++)...} {}
public:
	template<Convertible<T>... Us> requires (sizeof...(Us) <= n)
	constexpr explicit(sizeof...(Us) == 1) Arr(Us&&... us)
	: raw{SUP_FWD(us)...} {}
	template<RangeOf<T> R> constexpr explicit Arr(R&& range)
	: Arr{std::make_index_sequence<n>{}, std::ranges::begin(range)} {
		assert(std::ranges::size(range) >= n);
	}
	constexpr T const* begin() const { return raw; }
	constexpr T const* end() const { return raw + n; }
	constexpr T* begin() { return raw; }
	constexpr T* end() { return raw + n; }
	constexpr auto& operator[](std::size_t const i) const { return raw[i]; }
	constexpr auto& operator[](std::size_t const i) { return raw[i]; }
	template<auto i> friend constexpr auto&& get(Fwd<Arr> auto&& self) {
		static_assert(i < n, "get(Arr<>) index out of bounds");
		return forward_as<decltype(self)>(self[i]);
	}
};
template<typename T, typename... Ts> Arr(T&&, Ts&&...) ->
	Arr<std::remove_cvref_t<T>, sizeof...(Ts) + 1>;
} // namespace sup

namespace std {
template<typename T, auto n> struct tuple_size<sup::Arr<T, n>> {
	static auto constexpr value = n;
};
template<auto i, typename T, auto n> struct tuple_element<i, sup::Arr<T, n>> {
	static_assert(i < n, "tuple_element<Arr<>> index out of bounds");
	using type = T;
};
} // namespace std