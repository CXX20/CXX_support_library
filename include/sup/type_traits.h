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
	bool constexpr which[]{std::same_as<T, Us>...};
	for (auto& b: which) if (b) return &b - which;
}();
} // namespace pack

template<typename T, typename... As> concept Constructible =
	requires(As&&... args) { T{SUP_FWD(args)...}; };
template<typename T, typename U> concept Convertible =
	requires(T t) { U{t}; };
template<typename T, typename U> concept Fwd =
	std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>> &&
	Convertible<T&&, U>;

template<typename T> std::type_identity<T> constexpr type_v;

template<typename T> concept Typish = requires { type_v<typename T::type>; };

template<auto t> using Value = std::integral_constant<decltype(t), t>;
template<auto t> Value<t> constexpr value_v;

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