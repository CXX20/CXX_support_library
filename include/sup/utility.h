#pragma once

#include "type_traits.h"
#include <cassert>
#include <exception>
#include <memory>
#include <utility>

#define SUP_FWD(...) (::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__))

namespace sup {
/// The core C++20 `t != u` -> `!(t == u)` conversion requires
/// `{ t == u; } -> same_as<bool>`. This function is a polymorphic analogue.
template<typename T, typename U> constexpr auto operator!=(T&& t, U&& u)
requires requires { SUP_FWD(t) == SUP_FWD(u); } {
	return !(SUP_FWD(t) == SUP_FWD(u));
} // TODO remove explicit `template`

template<typename T> class Uninit {
	std::aligned_storage_t<sizeof(T), alignof(T)> storage;
public:
	void init(auto&&... args) requires requires { T{SUP_FWD(args)...}; } {
		new (&storage) T{SUP_FWD(args)...};
	}
	T& get() & { return *std::launder(reinterpret_cast<T*>(&storage)); }
};

template<typename... Fs> struct Overload: Fs... { using Fs::operator()...; };
template<typename... Fs> Overload(Fs...) -> Overload<Fs...>;

template<typename F> class Defer {
	F action;
public:
	Defer(Convertible<F> auto&& src): action{SUP_FWD(src)} {}
	Defer(Defer const&) = delete;
	~Defer() noexcept(noexcept(action())) { action(); }
};
template<typename F> Defer(F&&) -> Defer<F>;

template<typename C, typename M> class Member {
	M C::* raw;

public:
	consteval Member(M C::* p): raw{p} { if (!p) throw std::exception{}; }

	constexpr decltype(auto) operator()(auto&& instance, auto&&... args) const
	requires requires { (instance.*raw)(SUP_FWD(args)...); } {
		return (instance.*raw)(SUP_FWD(args)...);
	}
	constexpr auto&& operator()(Fwd<C> auto&& instance) const
	requires std::is_member_object_pointer_v<M C::*> {
		return SUP_FWD(instance).*raw;
	}
	
	constexpr auto get() const { return raw; }
	explicit constexpr operator M C::*() const { return get(); }
};
} // namespace sup