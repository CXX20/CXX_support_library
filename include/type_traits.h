#pragma once

#include <concepts>
#include <type_traits>

namespace sup {
template<typename T>
using RemoveCRef = std::remove_const_t<std::remove_reference_t<T>>;

template<typename T, typename U>
concept Forwarded = std::constructible_from<U, T&&> &&
	std::same_as<RemoveCRef<T>, RemoveCRef<U>>;

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
auto constexpr detected = Detect<T, C>::value;
} // namespace sup