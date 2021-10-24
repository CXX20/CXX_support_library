#pragma once

#include "array.h"

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

template<typename T, typename... Ts>
constexpr auto visit(auto&& fn, Enum<T, Ts...> const& enum_)
-> decltype(SUP_FWD(fn)(T{})) {
	Uninit<decltype(SUP_FWD(fn)(T{}))> ret;
	auto const seq = std::index_sequence_for<T, Ts...>{};
	return [&]<auto... is>(auto&& fn, std::index_sequence<is...>, auto... us)
			-> decltype(auto) {
		if ((... || (enum_.index() == is && (
			(SUP_FWD(fn)(typename decltype(us)::type{}), ret), true
		)))) return *ret;
		assert(false);
	}(SUP_FWD(fn), seq, type_v<T>, type_v<Ts>...);
}

template<typename T, typename E> class EnumArr: public Arr<T, E::size()> {
	using Base = Arr<T, E::size()>;
public:
	constexpr EnumArr(auto&&... ts)
	requires requires { Base{SUP_FWD(ts)...}; }
	: Base{SUP_FWD(ts)...} {}
	constexpr EnumArr(Typish auto, auto&&... ts)
	requires requires { EnumArr{SUP_FWD(ts)...}; }
	: EnumArr{SUP_FWD(ts)...} {}
	constexpr auto& operator[](E const& e) const {
		return Base::operator[](e.index());
	}
	constexpr auto& operator[](E const& e) {
		return Base::operator[](e.index());
	}
};
template<Typish E, typename T, typename... Ts> EnumArr(E, T&&, Ts&&...) ->
	EnumArr<std::remove_cvref_t<T>, typename E::type>;
} // namespace sup

namespace std {
template<typename T, typename E> struct tuple_size<sup::EnumArr<T, E>>
: public tuple_size<sup::Arr<T, E::size()>> {};
template<auto i, typename T, typename E>
struct tuple_element<i, sup::EnumArr<T, E>>
: public tuple_element<i, sup::Arr<T, E::size()>> {};
} // namespace std