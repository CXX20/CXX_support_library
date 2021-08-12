#pragma once

#include "type_traits.h"
#include <utility>

#define SUP_FWD(...) (::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__))

namespace sup {
template<typename T, typename U> constexpr auto operator!=(T&& a, U&& b)
requires requires { SUP_FWD(a) == SUP_FWD(b); } {
	return !(SUP_FWD(a) == SUP_FWD(b));
} // TODO remove explicit `template`

template<typename T> std::type_identity_t<T> constexpr type_v;

template<auto v> struct Value: std::integral_constant<decltype(v), v> {};
template<auto v> Value<v> constexpr value_v;

template<typename... Fs> struct Overload: public Fs... {
	using Fs::operator()...;
};

template<typename F> class Defer {
	F const action;
public:
	Defer(Fwd<F> auto&& src): action{SUP_FWD(src)} {}
	~Defer() noexcept(noexcept(action())) { action(); }
};
template<typename F> Defer(F&&) -> Defer<F>;

template<typename T>
struct [[deprecated("use C++20 `std::type_identity_t` instead")]] Type {
	using type = T;
};
} // namespace sup