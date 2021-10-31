#include <sup/array.h>

namespace {
sup::Arr constexpr arr{1, 2, 3};
static_assert(std::same_as<sup::ValueType<decltype(arr)>, int>);
static_assert(std::ranges::size(arr) == 3);
static_assert(arr[0] == 1);
static_assert(arr[1] == 2);
static_assert(arr[2] == 3);
static_assert(&std::ranges::begin(arr)[0] == &arr[0]);
static_assert(&std::ranges::begin(arr)[1] == &arr[1]);
static_assert(&std::ranges::begin(arr)[2] == &arr[2]);
static_assert(std::ranges::begin(arr) + 3 == std::ranges::end(arr));
static_assert(std::tuple_size_v<decltype(arr)> == 3);
static_assert(std::same_as<decltype(get<0>(arr)), int const&>);
static_assert(std::same_as<decltype(get<1>(arr)), int const&>);
static_assert(std::same_as<decltype(get<2>(arr)), int const&>);

sup::Arr<int, 0> constexpr brr;
static_assert(std::same_as<sup::ValueType<decltype(brr)>, int>);
static_assert(std::ranges::size(brr) == 0);
static_assert(std::ranges::begin(brr) == std::ranges::end(brr));
static_assert(std::tuple_size_v<decltype(brr)> == 0);
} // namespace