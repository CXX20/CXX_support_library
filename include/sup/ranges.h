#pragma once

#include "type_traits.h"
#include <stdexcept>

namespace sup { // TODO remove `impl` concepts
namespace impl {
template<typename R> concept HasBeginMember = requires(R r) { r.begin(); };
template<typename R> concept HasBeginAdl = requires(R r) { begin(r); };
template<typename R> concept HasBegin = HasBeginMember<R> || HasBeginAdl<R>;

template<typename R> concept HasEndMember = requires(R r) { r.end(); };
template<typename R> concept HasEndAdl = requires(R r) { end(r); };
template<typename R> concept HasEnd = HasEndMember<R> || HasEndAdl<R>;
} // namespace impl

constexpr auto begin(impl::HasBegin auto&& r) { return begin(r); }
constexpr auto begin(impl::HasBeginMember auto&& r) { return r.begin(); }
template<typename R, auto n> constexpr auto begin(R (&r)[n]) { return r; }

constexpr auto end(impl::HasEnd auto&& r) { return end(r); }
constexpr auto end(impl::HasEndMember auto&& r) { return r.end(); }
template<typename R, auto n> constexpr auto end(R (&r)[n]) { return r + n; }

namespace impl {
template<typename R> concept HasSizeMember = requires(R r) { r.size(); };
template<typename R> concept HasSizeAdl = requires(R r) { size(r); };
template<typename R> concept HasSizeIt = requires(R r) { end(r) - begin(r); };
template<typename R> concept HasSizeFn = HasSizeMember<R> || HasSizeAdl<R>;
template<typename R> concept HasSize = HasSizeFn<R> || HasSizeIt<R>;
} // namespace impl

constexpr auto size(impl::HasSize auto&& r) { return end(r) - begin(r); }
constexpr auto size(impl::HasSizeFn auto&& r) { return size(r); }
constexpr auto size(impl::HasSizeMember auto&& r) { return r.size(); }

template<typename R> using BeginOf = decltype(sup::begin(std::declval<R>()));
template<typename R> using EndOf = decltype(sup::end(std::declval<R>()));
template<typename R> using SizeOf = decltype(sup::size(std::declval<R>()));

template<typename R> concept Range =
	requires(R r) { sup::begin(r); } && requires(R r) { sup::end(r); };
template<typename R> concept BidirRange =
	Range<R> && requires(BeginOf<R> it) { --it; };
template<typename R> concept RaRange =
	Range<R> && requires(R r) { r[SizeOf<R>{}]; };

constexpr auto empty(Range auto&& r) { return sup::begin(r) == sup::end(r); }

constexpr auto rbegin(BidirRange auto&& r) {
	return std::reverse_iterator{sup::end(r)};
} // TODO prefer `r*` members/ADL
constexpr auto rend(BidirRange auto&& r) {
	return std::reverse_iterator{sup::begin(r)};
}
constexpr auto cbegin(Range auto const& r) { return sup::begin(r); }
constexpr auto cend(Range auto const& r) { return sup::end(r); }
constexpr auto crbegin(BidirRange auto const& r) { return sup::rbegin(r); }
constexpr auto crend(BidirRange auto const& r) { return sup::rend(r); }

constexpr decltype(auto) front(Range auto&& r) { return *sup::begin(r); }
constexpr decltype(auto) back(BidirRange auto&& r) { return *sup::rbegin(r); }
template<RaRange R> constexpr decltype(auto) at(R&& r, SizeOf<R> const idx) {
	return idx < sup::size(r) ? r[idx] : throw std::out_of_range{"sup::at"};
} // TODO remove explicit `template`
} // namespace sup