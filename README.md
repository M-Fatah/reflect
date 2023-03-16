<!-- badges: start -->
[![Build status](https://github.com/M-Fatah/reflect/workflows/CI/badge.svg)](https://github.com/M-Fatah/reflect/actions?workflow=CI)
<!-- badges: end -->
---

## **Introduction:**
Reflect is single file header only library that provides static reflection for C++.

## **API:**
Primitive types, pointers, arrays and enums are supported out of the box.
However automatic generation of reflection info for enums is limited to values between `REFLECT_MIN_ENUM_VALUE` to `REFLECT_MAX_ENUM_VALUE`.

```C++
struct Vector3
{
    f32 x, y, z;
};

TYPE_OF(Vector3, x, y, z)

template <typename T>
struct Foo
{
    T t;
};

template <typename T>
TYPE_OF(Foo<T>, t)

template <typename T, typename R>
struct Bar
{
    Foo<T> foo;
    R r;
};

template <typename T, typename R>
TYPE_OF((Bar<T, R>), foo, r)

struct Serializable
{
    char a;
    bool b;
    f32 *c;
    const char *d;
    void *e;
};

TYPE_OF(Serializable, a, b, c, d, (e, "NoSerialize"))
------------------------------------------------------------------------------
enum Enum
{
    A = -3,
    B = 8,
    c = 7
};

// Will return type info along enum values in this order {{"A", -3}, {"C", 7}, {"B", 8}}.
auto enum_type = type_of<Enum>();

// However, if `TYPE_OF_ENUM` macro is used, the correct order of definition will be returned.
// {{"A", -3}, {"B", 8}, {"C", 7}}
// And it can work on any enum range.
TYPE_OF_ENUM(Enu, A, B, C)
------------------------------------------------------------------------------
auto t = type_of<T>();
auto t = type_of(T{});
auto n = name_of<T>();
auto k = kind_of<T>();
auto v = value_of(T{});
```

## **Compilers:**
- MSVC: `-std:c++20 -Zc:preprocessor`.
- GCC: `-std=c++2b`.
- Clang (later).

## **Building:**
- Use one of the scripts to build examples and unittest.
- Output is in `build/bin/${CONFIG}/` directory.