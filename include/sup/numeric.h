#pragma once

#include <utility>

namespace sup {
template<typename T> class Wrapper {
	T raw;
public:
	template<std::same_as<T> U = T> constexpr Wrapper(U src = {}): raw{src} {}
	template<typename U> constexpr Wrapper(Wrapper<U> const& other)
	requires requires { T{other.get()}; }
	: raw{other.get()} {}
	constexpr auto get() const { return raw; }
	constexpr auto operator<=>(Wrapper<T> const&) const = default;
	explicit constexpr operator T() const requires (!std::same_as<T, bool>) {
		return raw;
	}
	explicit constexpr operator bool() const { return !!raw; }
};
template<typename T> Wrapper(T) -> Wrapper<T>;

template<typename T>
constexpr auto operator+(Wrapper<T> const& a, Wrapper<T> const& b)
requires requires { a.get() + b.get(); } {
	if constexpr (std::unsigned_integral<T> && sizeof(T) < sizeof(int))
		return Wrapper{static_cast<T>( // force safe promotion arithmetics
				(unsigned{a.get()} + unsigned{b.get()}) %
					(std::numeric_limits<T>::max() + 1u))};
	else // uint/ul/ull (no promotion) or signed (mustn't overflow anyway)
		return Wrapper{static_cast<T>(a.get() + b.get())};
}

using Bool = Wrapper<bool>;
using Int = Wrapper<int>;
using Unsigned = Wrapper<unsigned>;
using SizeT = Wrapper<std::size_t>;
} // namespace sup