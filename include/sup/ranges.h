#pragma once

#include "utility.h"
#include <ranges>

namespace sup {
template<typename R, typename E> concept RangeOf = requires(R range) {
	requires Convertible<decltype(*std::ranges::begin(range)), E>;
};

template<typename R> using SizeType =
	decltype(std::ranges::size(std::declval<R>()));
template<typename R> using ValueType =
	std::remove_cvref_t<decltype(*std::ranges::begin(std::declval<R&>()))>;

template<typename R> class RaiiSize {
	SizeType<R> raw;
public:
	constexpr RaiiSize(decltype(raw) const src = {}): raw{src} {}
	constexpr RaiiSize(RaiiSize const&) = default;
	constexpr RaiiSize(RaiiSize&&) noexcept
	requires std::is_trivially_move_constructible_v<R>
	= default;
	constexpr RaiiSize(RaiiSize&& other) noexcept
	requires (!std::is_trivially_move_constructible_v<R>)
	: raw{other.raw} { other.raw = {}; }
	constexpr RaiiSize& operator=(RaiiSize const&) = default;
	constexpr RaiiSize& operator=(RaiiSize&&) noexcept
	requires std::is_trivially_move_assignable_v<R>
	= default;
	constexpr RaiiSize& operator=(RaiiSize&& other) noexcept
	requires (!std::is_trivially_move_assignable_v<R>) {
		using std::swap;
		return (void)swap(raw, other.raw), *this;
	}
	constexpr auto& get() const& { return raw; }
	constexpr auto& get() & { return raw; }
	constexpr operator decltype(raw) const&() const& { return get(); }
	constexpr operator decltype(raw)&() & { return get(); }
};
} // namespace sup

namespace std {
template<typename R> struct numeric_limits<sup::RaiiSize<R>>
: public numeric_limits<sup::SizeType<R>> {};
} // namespace std