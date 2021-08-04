#pragma once

#include "type_traits.h"
#include <utility>

#define SUP_FWD(...) (::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__))

namespace sup {
template<typename T, typename U> constexpr auto operator!=(T&& lhs, U&& rhs)
requires std::equality_comparable_with<T&&, U&&> {
	return !(SUP_FWD(lhs) == SUP_FWD(rhs));
}

template<typename T> struct Type { using type = T; };
template<typename T> Type<T> constexpr type_v;

template<auto v> struct Value { static auto constexpr value = v; };
template<auto v> Value<v> constexpr value_v;

template<typename... Fs> struct Overload: public Fs... {
	using Fs::operator()...;
};

template<typename F> class Defer {
	F action;
public:
	Defer(Fwd<F> auto&& src): action{SUP_FWD(src)} {}
	Defer(Defer const&) = delete;
	~Defer() noexcept(noexcept(action())) { action(); }
};
template<typename F> Defer(F&&) -> Defer<F>;
} // namespace sup