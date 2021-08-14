#pragma once

#include <concepts>
#include <type_traits>

namespace sup {
template<typename T, typename U>
concept Fwd = std::constructible_from<U, T&&> &&
	std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template<typename T> std::type_identity<T> constexpr type_v;

template<auto v> using Value = std::integral_constant<decltype(v), v>;
template<auto v> Value<v> constexpr value_v;

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