#pragma once

#include "type_traits.h"
#include <exception>
#include <functional>
#include <memory>
#include <ranges>
#include <utility>

namespace sup {
template<typename R, typename E> concept RangeOf = requires(R range) {
	requires Convertible<decltype(*std::ranges::begin(range)), E>;
};

template<typename T> constexpr auto&& forward_as(auto&& u) {
	if constexpr (std::is_lvalue_reference_v<T>) {
		static_assert(!std::is_rvalue_reference_v<decltype(u)>);
		return u;
	} else return std::move(u);
}

/// The core C++20 `t != u` -> `!(t == u)` conversion requires
/// `{ t == u; } -> same_as<bool>`. This function is a polymorphic analogue.
template<typename T, typename U> constexpr auto operator!=(T&& t, U&& u)
requires requires { SUP_FWD(t) == SUP_FWD(u); } {
	return !(SUP_FWD(t) == SUP_FWD(u));
} // TODO remove raw `template`

template<typename... Fs> struct Overload: Fs... { using Fs::operator()...; };

template<typename F> class Defer {
	F action;
public:
	constexpr Defer(Convertible<F> auto&& src): action{SUP_FWD(src)} {}
	Defer(Defer const&) = delete;
	constexpr ~Defer() noexcept(noexcept(action())) { action(); }
};
template<typename F> Defer(F&&) -> Defer<F>;

template<typename C, typename M> class Member {
	M C::* raw;
public:
	consteval Member(M C::* const p): raw{p} { if (!p) throw std::exception{}; }
	constexpr auto&& operator()(Fwd<C> auto&& instance) const {
		return SUP_FWD(instance).*raw;
	}
	constexpr auto get() const { return raw; }
	constexpr explicit operator M C::*() const { return get(); }
};
} // namespace sup