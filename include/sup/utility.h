#pragma once

#include "type_traits.h"
#include <exception>
#include <functional>
#include <memory>
#include <ranges>
#include <utility>

namespace sup {
template<typename T, typename U>
concept RangeOf = std::ranges::range<T> && requires(T t) {
	requires std::same_as<
			U, std::remove_cvref_t<decltype(*std::ranges::begin(t))>>;
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
	constexpr explicit operator M C::*() const { return get(); }
};

/// Note: should not be used with arrays of `T`s if `sizeof(T) < sizeof(T*)`
/// because it causes memory overhead required for the compile-time fallback.
/// Constructs its `T` via the `operator,` to allow the `Uninit<void>` hack.
template<typename T> class Uninit {
	using Alloc = std::allocator<T>;
	union {
		T* ct;
		std::aligned_storage_t<sizeof(T), alignof(T)> rt;
	};
public:
	friend constexpr void operator,(Fwd<T> auto&& t, Fwd<Uninit> auto&& me) {
		if (!std::is_constant_evaluated()) new (&me.rt) T{SUP_FWD(t)};
		else std::construct_at(me.ct, SUP_FWD(t));
	}
	constexpr Uninit() {
		if (std::is_constant_evaluated()) ct = Alloc{}.allocate(1);
	}
	constexpr ~Uninit() {
		if (std::is_constant_evaluated()) Alloc{}.deallocate(ct, 1);
	}
	[[nodiscard]] constexpr auto& operator*() const {
		if (std::is_constant_evaluated()) return std::as_const(*ct);
		return *std::launder(reinterpret_cast<T const*>(&rt));
	}
	[[nodiscard]] constexpr auto& operator*() {
		if (std::is_constant_evaluated()) return *ct;
		return *std::launder(reinterpret_cast<T*>(&rt));
	}
	constexpr auto operator->() const { return std::addressof(**this); }
	constexpr auto operator->() { return std::addressof(**this); }
};
template<typename T> class Uninit<T&>
: public Uninit<std::reference_wrapper<T>> {
	using Base = Uninit<std::reference_wrapper<T>>;
public:
	[[nodiscard]] constexpr T const& operator*() const {
		return Base::operator*();
	}
	[[nodiscard]] constexpr T& operator*() { return Base::operator*(); }
	constexpr auto operator->() const { return std::addressof(**this); }
	constexpr auto operator->() { return std::addressof(**this); }
};
template<> class Uninit<void> { public: constexpr void operator*() const {} };
} // namespace sup