# C++ Support Library (SUP)

## Motivation

Most of the time C++ is used (and therefore considered) as something either low-level and unsafe (C-style) or slow and wordy (C with classes/Java-style). However, when used properly, it can be faster and more expressive than any other mainstream language. This library tries to make finding that "proper usage" easier while writing as little code for it as possible.

## Features
* [enum](#enum)
* [numeric](#numeric)
* [utility](#utility)
* [type_traits](#type_traits)

### enum <a name="enum"></a>
Zero-cost, type-safe, aware of own size and elements:
```c++
//enum class Mood { bad, alright, great };
//std::size_t constexpr mood_size{3};                      // repeats itself
//std::array moods{Mood::bad, Mood::alright, Mood::great}; // repeats itself again

struct Bad {};
struct Alright {};
struct Great {};
using Mood = sup::Enum<Bad, Alright, Great>;

static_assert(Mood::find_v<Bad> == 0);

// somewhere else
visit(sup::Overload{ // see "utility"
	[](Bad)     { std::cout << ":("; },
	[](Alright) { std::cout << ":|"; },
	[](Great)   { std::cout << ":)"; }
}, Mood{Great{}});
```

### numeric <a name="numeric"></a>
Zero-cost type-safe wrappers for C arithmetic types:
```c++
//-2 < 1u; // `false` with raw C types, compile-time error with SUP wrappers
//unsigned short s1 = 0, s2 = 1, s3 = s1 - s2; // UB with C types, compile-time error with SUP

sup::U8 u8 = 42_c; // for `_c` see Boost.Hana
sup::U16 u16 = u8;
sup::I32 i32 = u16;
```

### utility <a name="utility"></a>
`defer` a la Golang but implemented as a library feature instead of being a built-in language construct:
```c++
sup::Defer cleanup{foo};
```
Statically enforced non-nullable member pointers:
```c++
struct Test { int field{}; } test;
sup::Member field{&Test::field};
field = nullptr; // compile-time error
field = {};      // compile-time error
field(test); // `test.*field`
```
Uninitialized-by-default values which work polymorphically even for `constexpr`/`T&`/`void`.

### type_traits <a name="type_traits"></a>
Variadic template utilities - BTW, optimized for faster compilation (e.g. avoids recursion):
```c++
static_assert(std::same_as<sup::pack::At<2, void, char, int>, int>);
static_assert(sup::pack::find_v<int, void, char, int> == 2);
```
More convenient typelevel values with type deduction:
```c++
//std::integral_constant<unsigned long, 42>
Value<42ul>
```

## Installation <a name="installation"></a>
Just download `/include/` and mark it as an include directory, then use normally:
```c++
#include <sup/utility.h>
```
