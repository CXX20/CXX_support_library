Most of the time C++ is used (and therefore considered) as something either low-level and unsafe (C-style) or slow and wordy (C with classes/Java-style). However, when used properly, it can be faster and more expressive than any other mainstream language. This library tries to make finding that "proper usage" easier while "spending" on it as least code as possible.

C++ Support Library (SUP) features:

# enum
### Zero-cost enum aware of own size and elements
Replace your "unsynchronized" hacks like
```
enum class Mood { bad, alright, great };
std::size_t constexpr mood_size{3};                     // repeats itself
std::array moods{Mood::bad, Mood::alright, Mood::great}; // repeats itself again
```
...with copypasteless
```
struct Bad {};
struct Alright {};
struct Great {};
using Mood = sup::Enum<Bad, Alright, Great>;
```
Now everything about your `Mood`s is automatically stored at the type level:
```
static_assert(Mood::find_v<Bad> == 0);
```
Matching their values is still simple:
```
visit(sup::Overload{ // see "utility"
	[](Bad)     { std::cout << ":("; },
	[](Alright) { std::cout << ":|"; },
	[](Great)   { std::cout << ":)"; }
}, Mood{Great{}});
```

# numeric
### Zero-cost type-safe wrappers for C arithmetic types
No more bugs like
```
-2 < 1u; // `false` with raw C types, compile-time error with SUP wrappers
unsigned short s1 = 0, s2 = 1, s3 = s1 - s2; // undefined behavior with raw C types, compile-time error with SUP wrappers
```
However, no manual work is required for safe casts:
```
sup::U8 u8 = 42_c; // for `_c` see Boost.Hana
sup::U16 u16 = u8;
sup::I32 i32 = u16;
```

# utility
### `defer` a la Golang
Implemented as a library instead of being a language feature.
```
sup::Defer _{foo};
```
### Statically enforced non-nullable member pointers
```
struct Test { int field{}; } test;
sup::Member field{&Test::field};
field = nullptr; // compile-time error
field = {};      // compile-time error
field(test); // `test.*field`
```

# type_traits
### Variadic template utilities
BTW, optimized for faster compilation (e.g. avoids recursion).
```
static_assert(std::same_as<sup::pack::At<2, void, char, int>, int>);
static_assert(sup::pack::find_v<int, void, char, int> == 2);
```
### More convenient typelevel values
Deduce the type instead of repeating yourself:
```
std::integral_constant<unsigned long, 42>;
Value<42ul>;
```
