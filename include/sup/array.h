#pragma once

#include "utility.h"
#include <cassert>

namespace sup {
template<typename T, std::size_t n> class Arr {
	T raw[n ? n : 1];
	template<auto... is> constexpr Arr(std::index_sequence<is...>, auto iter)
	: raw{((void)is, *iter++)...} {}
public:
	template<Convertible<T>... Us> requires (sizeof...(Us) <= n)
	constexpr explicit(sizeof...(Us) == 1) Arr(Us&&... us)
	: raw{SUP_FWD(us)...} {}
	constexpr explicit Arr(RangeOf<T> auto&& range)
	: Arr{std::make_index_sequence<n>{}, std::ranges::begin(range)} {
		assert(std::ranges::size(range) >= n);
	}
	constexpr auto& operator[](std::size_t const i) const { return raw[i]; }
	constexpr auto& operator[](std::size_t const i) { return raw[i]; }
	friend constexpr auto begin(Fwd<Arr> auto& self) { return self.raw; }
	friend constexpr auto end(Fwd<Arr> auto& self) { return self.raw + n; }
	template<auto i> friend constexpr auto&& get(Fwd<Arr> auto&& self) {
		static_assert(i < n);
		return forward_as<decltype(self)>(self[i]);
	}
};
template<typename T, typename... Ts> Arr(T&&, Ts&&...) ->
	Arr<std::remove_cvref_t<T>, sizeof...(Ts) + 1u>;
} // namespace sup

namespace std {
template<typename T, auto n> struct tuple_size<sup::Arr<T, n>> {
	static auto constexpr value = n;
};
template<auto i, typename T, auto n> struct tuple_element<i, sup::Arr<T, n>> {
	static_assert(i < n);
	using type = T;
};
} // namespace std