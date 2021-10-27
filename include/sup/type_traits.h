#pragma once

#include <concepts>
#include <type_traits>
#include <tuple>

#define SUP_FWD(...) (::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__))

namespace sup {
namespace pack {
template<typename T, typename... Us> concept OneOf =
	(... || std::same_as<T, Us>);

template<auto i, typename... Ts> using At =
	std::tuple_element_t<i, std::tuple<Ts...>>;

template<typename T, typename... Us> auto constexpr find_v = [] {
	bool constexpr all[]{std::same_as<T, Us>...};
	for (auto& b: all) if (b) return &b - all;
}();
} // namespace pack

template<auto t> using Value = std::integral_constant<decltype(t), t>;
template<auto t> Value<t> constexpr value_v;
template<typename T> std::type_identity<T> constexpr type_v;

template<typename T, typename... As> concept Constructible =
	requires(As&&... args) { T{SUP_FWD(args)...}; };
template<typename T, typename U> concept Convertible =
	requires(T&& t) { U{SUP_FWD(t)}; };
template<typename T, typename U> concept Fwd = Convertible<T, U> && (
		std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>> ||
		std::derived_from<std::remove_cvref_t<T>, std::remove_cvref_t<U>>);
template<typename T> concept Typish = requires { type_v<typename T::type>; };

template<typename T, template<typename...> typename C>
class [[deprecated("use C++20 `requires` instead")]] Detect {
	static constexpr auto test(...) { return std::false_type{}; }
	template<typename = C<T>> static constexpr auto test(int) {
		return std::true_type{};
	}
public:
	static bool constexpr value{test(42)};
};
template<typename T, template<typename...> typename C>
auto constexpr detect_v = Detect<T, C>::value;
} // namespace sup