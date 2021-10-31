#include <sup/ranges.h>
#include <array>
#include <string>

static_assert(sup::RangeOf<std::string, char>);
static_assert(sup::RangeOf<std::string, long long>);
static_assert(std::same_as<sup::ValueType<std::string>, char>);
static_assert(std::same_as<sup::SizeType<std::string>, std::string::size_type>);

static_assert([] {
	struct Str2D {
		std::string src;
		sup::RaiiSize<decltype(src)> width;
	} s1{"1212", 2}, s2{std::move(s1)};
	return s1.width == 0 && s2.width == 2;
}());

static_assert([] {
	struct TrivialStr2D {
		std::array<char, 4> src;
		sup::RaiiSize<decltype(src)> width;
	} s1{{'1', '2', '1', '2'}, 2}, s2{std::move(s1)};
	return s1.width == 2 && s2.width == 2;
}());