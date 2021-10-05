#pragma once

#include "utility.h"
#include <new>

namespace sup {
template<typename... Ts> class Enum {
	static_assert(sizeof...(Ts) <= UINT_LEAST8_MAX, "too many enumerators");
	std::uint_least8_t tag{};
public:
	template<typename U> static decltype(tag) constexpr find_v =
		pack::find_v<U, Ts...>;
	constexpr Enum() {}
	template<pack::OneOf<Ts...> U> constexpr Enum(U const&)
	: tag{pack::find_v<U, Ts...>} {}
	constexpr auto index() const { return tag; }
	static constexpr decltype(tag) size() { return sizeof...(Ts); }
};

template<typename... Ts> auto visit(auto fn, Enum<Ts...> const& enum_) {
	return [&]<std::size_t... is>(std::index_sequence<is...>) {
		using Ret = decltype(fn(pack::At<0, Ts...>{}));
		static_assert(!std::is_reference_v<Ret> && !std::is_void_v<Ret>);
		std::aligned_storage_t<sizeof(Ret), alignof(Ret)> ret;
		auto case_ = [&]<auto t>(Value<t>) {
			if (enum_.index() != t) return false;
			new (&ret) Ret{fn(pack::At<t, Ts...>{})};
			return true;
		};
		if ((... || case_(value_v<is>))) {}
		return *std::launder(reinterpret_cast<Ret*>(&ret));
	}(std::make_index_sequence<sizeof...(Ts)>{});
}
} // namespace sup