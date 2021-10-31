#include <sup/optional.h>

namespace {
static_assert([] {
	auto i = 42, j = 322;
	sup::Opt<int&> opt{i};
	if (!opt || *opt != 42) throw;
	if (opt = j; !opt || *opt != 322) throw;
	if (*opt = 1337; i != 42 || j != 1337) throw;
	return true;
}());

static_assert([] {
	sup::Opt<bool> opt;
	if (opt) throw;
	if (opt = false; !opt || *opt) throw;
	if (*opt = true; !opt || !*opt) throw;
	return true;
}());

auto constexpr i = 42;
sup::Opt<int const&> constexpr opt{i};
static_assert(opt);
static_assert(*opt == 42);
} // namespace