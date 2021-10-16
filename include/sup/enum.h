#pragma once

#include "utility.h"

namespace sup {
template<typename... Ts> class Enum {
	static_assert(sizeof...(Ts) <= UINT_LEAST8_MAX, "too many enumerators");
	std::uint_least8_t tag{};
public:
	static constexpr auto size() {
		return std::integral_constant<decltype(tag), sizeof...(Ts)>{};
	}
	template<typename U> static decltype(tag) constexpr find_v =
		pack::find_v<U, Ts...>;
	constexpr Enum() {}
	template<pack::OneOf<Ts...> U> constexpr Enum(U const&)
	: tag{pack::find_v<U, Ts...>} {}
	constexpr auto index() const { return tag; }
};

template<typename... Ts> auto visit(auto fn, Enum<Ts...> const& enumeration) {
	return [&]<std::size_t... is>(std::index_sequence<is...>) {
		Uninit<decltype(fn(pack::At<0, Ts...>{}))> ret;
		if ((... || (enumeration.index() == is ?
				ret.init(fn(pack::At<is, Ts...>{})), true : false)))
			return ret.get();
		assert(false);
	}(std::make_index_sequence<sizeof...(Ts)>{});
}

template<typename T, typename E>
class EnumArr: public std::array<T, E::size()> {
	using Base = std::array<T, E::size()>;
public:
	constexpr EnumArr(auto&&... ts)
	requires requires { Base{SUP_FWD(ts)...}; }
	: Base{SUP_FWD(ts)...} {}
	constexpr EnumArr(Typish auto, auto&&... ts)
	requires requires { Base{SUP_FWD(ts)...}; }
	: EnumArr{SUP_FWD(ts)...} {}

	static constexpr auto size() { return E::size(); }

	constexpr auto& operator[](E const& e) const {
		return Base::operator[](e.index());
	}
	constexpr auto& operator[](E const& e) {
		return Base::operator[](e.index());
	}
	constexpr auto& at(E const& e) const { return (*this)[e.index()]; }
	constexpr auto& at(E const& e) { return (*this)[e.index()]; }
};
template<Typish E, typename... Ts> EnumArr(E type, Ts&&... ts) ->
	EnumArr<std::remove_cvref_t<pack::At<0, Ts...>>, typename E::type>;
} // namespace sup