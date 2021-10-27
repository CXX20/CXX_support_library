#pragma once

#include "utility.h"
#include <functional>
#include <memory>

namespace sup {
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
	friend constexpr void operator,(Fwd<T> auto&& t, Uninit& me) {
		me.emplace(SUP_FWD(t));
	}
	constexpr Uninit() {
		if (std::is_constant_evaluated()) ct = Alloc{}.allocate(1);
	}
	constexpr ~Uninit() {
		if (std::is_constant_evaluated()) ct->~T(), Alloc{}.deallocate(ct, 1);
	}
	constexpr void emplace(auto&&... args) {
		std::is_constant_evaluated() ? std::construct_at(ct, SUP_FWD(args)...) :
			new (&rt) T{SUP_FWD(args)...};
	}
	[[nodiscard]] constexpr auto& operator*() const {
		return std::is_constant_evaluated() ? std::as_const(*ct) :
			*std::launder(reinterpret_cast<T const*>(&rt));
	}
	[[nodiscard]] constexpr auto& operator*() {
		return std::is_constant_evaluated() ? *ct :
			*std::launder(reinterpret_cast<T*>(&rt));
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
template<typename T> requires std::same_as<std::remove_cv_t<T>, void>
struct Uninit<T> {
	constexpr void operator*() const {}
	constexpr void emplace() {}
};
} // namespace sup