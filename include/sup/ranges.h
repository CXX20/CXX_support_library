#pragma once

#include "type_traits.h"

namespace sup {
namespace impl {
template<typename T> concept HasBeginMember = requires(T t) { t.begin(); };
template<typename T> concept HasBeginAdl = requires(T t) { begin(t); };
template<typename T> concept HasBegin = HasBeginMember<T> || HasBeginAdl<T>;

template<typename T> concept HasEndMember = requires(T t) { t.end(); };
template<typename T> concept HasEndAdl = requires(T t) { end(t); };
template<typename T> concept HasEnd = HasEndMember<T> || HasEndAdl<T>;
} // namespace impl

constexpr auto begin(impl::HasBegin auto&& t) { return begin(t); }
constexpr auto begin(impl::HasBeginMember auto&& t) { return t.begin(); }
template<typename T, auto n> constexpr auto begin(T (&t)[n]) { return t; }

constexpr auto end(impl::HasEnd auto&& t) { return end(t); }
constexpr auto end(impl::HasEndMember auto&& t) { return t.end(); }
template<typename T, auto n> constexpr auto end(T (&t)[n]) { return t + n; }

namespace impl {
template<typename T> concept HasSizeMember = requires(T t) { t.size(); };
template<typename T> concept HasSizeAdl = requires(T t) { size(t); };
template<typename T> concept HasSizeIt = requires(T t) { end(t) - begin(t); };
template<typename T> concept HasSizeFn = HasSizeMember<T> || HasSizeAdl<T>;
template<typename T> concept HasSize = HasSizeFn<T> || HasSizeIt<T>;
} // namespace impl

constexpr auto size(impl::HasSize auto&& t) { return end(t) - begin(t); }
constexpr auto size(impl::HasSizeFn auto&& t) { return size(t); }
constexpr auto size(impl::HasSizeMember auto&& t) { return t.size(); }

template<typename T> using BeginOf = decltype(sup::begin(std::declval<T>()));
template<typename T> using EndOf = decltype(sup::end(std::declval<T>()));

template<typename T> concept Range =
	requires(T t) { sup::begin(t); } && requires(T t) { sup::end(t); };
template<typename T> concept BidirRange =
	Range<T> && requires(BeginOf<T> it) { --it; };

constexpr auto empty(Range auto&& t) { return sup::begin(t) == sup::end(t); }

constexpr auto rbegin(BidirRange auto&& t) {
	return std::reverse_iterator{sup::end(t)};
}
constexpr auto rend(BidirRange auto&& t) {
	return std::reverse_iterator{sup::begin(t)};
}
constexpr auto cbegin(Range auto const& t) { return sup::begin(t); }
constexpr auto cend(Range auto const& t) { return sup::end(t); }
constexpr auto crbegin(BidirRange auto const& t) { return sup::rbegin(t); }
constexpr auto crend(BidirRange auto const& t) { return sup::rend(t); }

constexpr decltype(auto) front(Range auto&& t) { return *sup::begin(t); }
constexpr decltype(auto) back(BidirRange auto&& t) { return *sup::rbegin(t); }

template<typename R> class View {
	BeginOf<R> from{};
	EndOf<R> to{};
public:
	constexpr View() {}
	constexpr View(BeginOf<R> const a, EndOf<R> const b): from{a}, to{b} {}
	constexpr View(R& range): View{sup::begin(range), sup::end(range)} {}
	constexpr auto begin() const { return from; }
	constexpr auto end() const { return to; }
	constexpr auto operator==(View<R> const& other) const {
		return from == other.from && to == other.to;
	}
};

template<typename T> class IntView {
	class End {
		friend class IntView;
		T limit;
		constexpr End(T const limit): limit{limit} {}
	};
	class Begin {
		friend class IntView;
		T cur{}, step{};
		constexpr Begin(T const cur, T const step): cur{cur}, step{step} {}
	public:
		constexpr auto operator*() const { return cur; }
		constexpr auto operator++() { return (void)(cur += step), *this; }
		constexpr auto operator==(End end) const { return cur >= end.limit; }
	};
	T from{}, to{}, step{};
public:
	constexpr IntView(T a, T b, T d = value_v<1>): from{a}, to{b}, step{d} {}
	constexpr IntView(T const to = {}): IntView{T{}, to} {}
	constexpr auto begin() const { return Begin{from, step}; }
	constexpr auto end() const { return End{to}; }
};
} // namespace sup