#pragma once

#include <array>
#include <string.h>
#include <stddef.h>
#include <string_view>
#include <type_traits>
#include <functional>

#define CONCATENATE(ARG1, ARG2) ARG1##ARG2

#define IF(CONDITION) CONCATENATE(_IF_, CONDITION)
#define _IF_0(TRUE, FALSE) FALSE
#define _IF_1(TRUE, FALSE) TRUE

#define HAS_PARENTHESIS(ARG) _HAS_PARENTHESIS_CHECK(_HAS_PARENTHESIS_PROBE ARG)
#define _HAS_PARENTHESIS_CHECK(...) _HAS_PARENTHESIS_CHECK_N(__VA_ARGS__, 0)
#define _HAS_PARENTHESIS_CHECK_N(ARG, N, ...) N
#define _HAS_PARENTHESIS_PROBE(...) 0, 1

// TODO: Rename to ARG_COUNT?
#define OVERLOAD_ARG_N_(                               \
     _01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
     _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
     _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
     _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
     _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
     _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
     _61, _62, _63, _64, N,...) N
#define OVERLOAD_RSEQ_N_()                             \
     64, 63, 62, 61, 60,                               \
     59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
     49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
     39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
     29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
     19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
     09, 08, 07, 06, 05, 04, 03, 02, 01, 00
#define OVERLOAD_RSEQ_N(...)    OVERLOAD_ARG_N_(__VA_ARGS__)
#define OVERLOAD_ARG_N(...)     OVERLOAD_RSEQ_N(__VA_ARGS__, OVERLOAD_RSEQ_N_())
#define OVERLOAD_(NAME, NUMBER) CONCATENATE(NAME, NUMBER)
#define OVERLOAD(NAME, ...)     OVERLOAD_(NAME, OVERLOAD_ARG_N(__VA_ARGS__))(__VA_ARGS__)

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef float     f32;
typedef double    f64;

typedef intptr_t  iptr;
typedef uintptr_t uptr;

/*
	TODO:
	- [x] Generate reflection
		- [x] Primitives.
			- [x] void?
		- [x] Pointers.
		- [x] Arrays.
		- [x] Structs and Classes.
			- [x] Empty structs and classes.
			- [ ] Abstract structs/classes?
			- [ ] Handle private variables inside classes by overloading member functions?
		- [x] Enums.
			- [x] Enum class?
			- [x] Macro helper.
				- [x] What if enum were used as flags?
				- [x] What if the user used weird assignment values (for example => ENUM_ZERO = 0, ENUM_THREE = 3, ENUM_TWO = 2)?
					- [x] Preserve the order of enum values?
				- [x] Enums with the same value?
				- [x] Enums with negative values?
			- [ ] Simplify type_of(Enum).
			- [ ] Add enum range?
		- [ ] Functions?
	- [x] name_of<T>().
		- [x] Names with const specifier.
		- [x] Pointer names.
		- [ ] Use local stack arrays and try to compute names compile times and store final names,
				in a static std::array<char> in a specialized template struct.
		- [ ] Simplify.
	- [ ] Make it compatible with C++17?
	- [ ] Cleanup.
*/

inline static constexpr const u64 REFLECT_MAX_NAME_LENGTH      = 128;
inline static constexpr const i32 REFLECT_MIN_ENUM_VALUE       = -32;
inline static constexpr const i32 REFLECT_MAX_ENUM_VALUE       =  32;
inline static constexpr const i32 REFLECT_MAX_ENUM_VALUE_COUNT = REFLECT_MAX_ENUM_VALUE - REFLECT_MIN_ENUM_VALUE;

enum TYPE_KIND
{
	TYPE_KIND_I8,
	TYPE_KIND_I16,
	TYPE_KIND_I32,
	TYPE_KIND_I64,
	TYPE_KIND_U8,
	TYPE_KIND_U16,
	TYPE_KIND_U32,
	TYPE_KIND_U64,
	TYPE_KIND_F32,
	TYPE_KIND_F64,
	TYPE_KIND_BOOL,
	TYPE_KIND_CHAR,
	TYPE_KIND_VOID,
	TYPE_KIND_POINTER,
	TYPE_KIND_ARRAY,
	TYPE_KIND_ENUM,
	TYPE_KIND_STRUCT
};

struct Type_Enum_Value
{
	i32 index;
	const char *name;
};

struct Type_Field
{
	const char *name;
	u64 offset;
	const struct Type *type;
	const char *tag;
};

struct Type
{
	const char *name;
	TYPE_KIND kind;
	u64 size;
	u64 align;
	union
	{
		struct
		{
			const Type *pointee;
		} as_pointer;
		struct
		{
			const Type *element;
			u64 element_count;
		} as_array;
		struct
		{
			const Type_Enum_Value *values;
			u64 value_count;
		} as_enum;
		struct
		{
			const Type_Field *fields;
			u64 field_count;
		} as_struct;
	};
};

struct Value
{
	const void *data;
	const Type *type;
};

inline static constexpr void
_reflect_append_name(char *name, u64 &count, std::string_view type_name)
{
	constexpr auto string_append = [](char *string, const char *to_append, u64 &count) {
		while(*to_append != '\0' && count < REFLECT_MAX_NAME_LENGTH - 1)
			string[count++] = *to_append++;
	};

	constexpr auto append_type_name_prettified = [string_append](char *name, std::string_view type_name, u64 &count) {
		if (type_name.starts_with(' '))
			type_name.remove_prefix(1);

		if (type_name.starts_with("const "))
		{
			string_append(name, "const ", count);
			type_name.remove_prefix(6);
		}

		#if defined(_MSC_VER)
			if (type_name.starts_with("enum "))
				type_name.remove_prefix(5);
			else if (type_name.starts_with("class "))
				type_name.remove_prefix(6);
			else if (type_name.starts_with("struct "))
				type_name.remove_prefix(7);
		#endif

		if (type_name.starts_with("signed char"))
		{
			string_append(name, "i8", count);
			type_name.remove_prefix(11);
		}
		else if (type_name.starts_with("short int"))
		{
			string_append(name, "i16", count);
			type_name.remove_prefix(9);
		}
		else if (type_name.starts_with("short") && !type_name.starts_with("short unsigned int"))
		{
			string_append(name, "i16", count);
			type_name.remove_prefix(5);
		}
		else if (type_name.starts_with("int"))
		{
			string_append(name, "i32", count);
			type_name.remove_prefix(3);
		}
		else if (type_name.starts_with("__int64"))
		{
			string_append(name, "i64", count);
			type_name.remove_prefix(7);
		}
		else if (type_name.starts_with("long int"))
		{
			string_append(name, "i64", count);
			type_name.remove_prefix(8);
		}
		else if (type_name.starts_with("unsigned char"))
		{
			string_append(name, "u8", count);
			type_name.remove_prefix(13);
		}
		else if (type_name.starts_with("unsigned short"))
		{
			string_append(name, "u16", count);
			type_name.remove_prefix(14);
		}
		else if (type_name.starts_with("short unsigned int"))
		{
			string_append(name, "u16", count);
			type_name.remove_prefix(18);
		}
		else if (type_name.starts_with("unsigned int"))
		{
			string_append(name, "u32", count);
			type_name.remove_prefix(12);
		}
		else if (type_name.starts_with("unsigned __int64"))
		{
			string_append(name, "u64", count);
			type_name.remove_prefix(16);
		}
		else if (type_name.starts_with("long unsigned int"))
		{
			string_append(name, "u64", count);
			type_name.remove_prefix(17);
		}
		else if (type_name.starts_with("float"))
		{
			string_append(name, "f32", count);
			type_name.remove_prefix(5);
		}
		else if (type_name.starts_with("double"))
		{
			string_append(name, "f64", count);
			type_name.remove_prefix(6);
		}

		for (char c : type_name)
			if (c != ' ')
				name[count++] = c;
	};

	bool add_pointer = false;
	bool add_const   = false;
	if (type_name.ends_with("* const"))
	{
		type_name.remove_suffix(7);
		add_pointer = true;
		add_const = true;
	}

	if (type_name.ends_with(" const "))
	{
		string_append(name, "const ", count);
		type_name.remove_suffix(7);
	}
	else if (type_name.ends_with(" const *"))
	{
		string_append(name, "const ", count);
		type_name.remove_suffix(8);
		add_pointer = true;
	}
	else if (type_name.ends_with('*'))
	{
		type_name.remove_suffix(1);
		add_pointer = true;
	}

	if (type_name.ends_with(' '))
		type_name.remove_suffix(1);

	if (type_name.ends_with('>'))
	{
		u64 open_angle_bracket_pos = type_name.find('<');
		append_type_name_prettified(name, type_name.substr(0, open_angle_bracket_pos), count);
		type_name.remove_prefix(open_angle_bracket_pos + 1);

		name[count++] = '<';
		u64 prev = 0;
		u64 match = 1;
		for (u64 c = 0; c < type_name.length(); ++c)
		{
			if (type_name.at(c) == '<')
			{
				++match;
			}

			if (type_name.at(c) == '>')
			{
				--match;
				if (match <= 0)
				{
					_reflect_append_name(name, count, type_name.substr(prev, c - prev));
					name[count++] = '>';
					prev = c + 1;
				}
			}

			if (type_name.at(c) == ',')
			{
				_reflect_append_name(name, count, type_name.substr(prev, c - prev));
				name[count++] = ',';
				prev = c + 1;
			}
		}
	}
	else
	{
		append_type_name_prettified(name, type_name, count);
	}

	if (add_pointer)
		name[count++] = '*';
	if (add_const)
		string_append(name, " const", count);
}

template <typename T>
inline static constexpr const char *
name_of()
{
		 if constexpr (std::is_same_v<T, i8>)   return "i8";
	else if constexpr (std::is_same_v<T, i16>)  return "i16";
	else if constexpr (std::is_same_v<T, i32>)  return "i32";
	else if constexpr (std::is_same_v<T, i64>)  return "i64";
	else if constexpr (std::is_same_v<T, u8>)   return "u8";
	else if constexpr (std::is_same_v<T, u16>)  return "u16";
	else if constexpr (std::is_same_v<T, u32>)  return "u32";
	else if constexpr (std::is_same_v<T, u64>)  return "u64";
	else if constexpr (std::is_same_v<T, f32>)  return "f32";
	else if constexpr (std::is_same_v<T, f64>)  return "f64";
	else if constexpr (std::is_same_v<T, bool>) return "bool";
	else if constexpr (std::is_same_v<T, char>) return "char";
	else if constexpr (std::is_same_v<T, void>) return "void";
	else
	{
		constexpr auto get_type_name = [](std::string_view type_name) -> const char * {
			static char name[REFLECT_MAX_NAME_LENGTH] = {};
			u64 count = 0;
			_reflect_append_name(name, count, type_name);
			return name;
		};

		#if defined(_MSC_VER)
			constexpr auto type_function_name      = std::string_view{__FUNCSIG__};
			constexpr auto type_name_prefix_length = type_function_name.find("name_of<") + 8;
			constexpr auto type_name_length        = type_function_name.rfind(">") - type_name_prefix_length;
		#elif defined(__GNUC__)
			constexpr auto type_function_name      = std::string_view{__PRETTY_FUNCTION__};
			constexpr auto type_name_prefix_length = type_function_name.find("= ") + 2;
			constexpr auto type_name_length        = type_function_name.rfind("]") - type_name_prefix_length;
		#else
			#error "[REFLECT]: Unsupported compiler."
		#endif
		static const char *name = get_type_name({type_function_name.data() + type_name_prefix_length, type_name_length});
		return name;
	}
}

template <typename T>
inline static constexpr TYPE_KIND
kind_of()
{
	if constexpr (std::is_same_v<T, i8>)
		return TYPE_KIND_I8;
	else if constexpr (std::is_same_v<T, i16>)
		return TYPE_KIND_I16;
	else if constexpr (std::is_same_v<T, i32>)
		return TYPE_KIND_I32;
	else if constexpr (std::is_same_v<T, i64>)
		return TYPE_KIND_I64;
	else if constexpr (std::is_same_v<T, u8>)
		return TYPE_KIND_U8;
	else if constexpr (std::is_same_v<T, u16>)
		return TYPE_KIND_U16;
	else if constexpr (std::is_same_v<T, u32>)
		return TYPE_KIND_U32;
	else if constexpr (std::is_same_v<T, u64>)
		return TYPE_KIND_U64;
	else if constexpr (std::is_same_v<T, f32>)
		return TYPE_KIND_F32;
	else if constexpr (std::is_same_v<T, f64>)
		return TYPE_KIND_F64;
	else if constexpr (std::is_same_v<T, bool>)
		return TYPE_KIND_BOOL;
	else if constexpr (std::is_same_v<T, char>)
		return TYPE_KIND_CHAR;
	else if constexpr (std::is_same_v<T, void>)
		return TYPE_KIND_VOID;
	else if constexpr (std::is_pointer_v<T>)
		return TYPE_KIND_POINTER;
	else if constexpr (std::is_array_v<T>)
		return TYPE_KIND_ARRAY;
	else if constexpr (std::is_enum_v<T>)
		return TYPE_KIND_ENUM;
	else if constexpr (std::is_compound_v<T>)
		return TYPE_KIND_STRUCT;
}

template <typename T>
inline static constexpr const Type *
type_of(const T)
{
	static_assert(sizeof(T) == 0, "There is no `inline static const Type * type_of(const T)` function overload defined for this type.");
	return nullptr;
}

template <typename T>
inline static constexpr const Type *
type_of();

#define _TYPE_OF_FIELD16(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD15(__VA_ARGS__)
#define _TYPE_OF_FIELD15(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD14(__VA_ARGS__)
#define _TYPE_OF_FIELD14(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD13(__VA_ARGS__)
#define _TYPE_OF_FIELD13(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD12(__VA_ARGS__)
#define _TYPE_OF_FIELD12(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD11(__VA_ARGS__)
#define _TYPE_OF_FIELD11(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD10(__VA_ARGS__)
#define _TYPE_OF_FIELD10(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD09(__VA_ARGS__)
#define _TYPE_OF_FIELD09(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD08(__VA_ARGS__)
#define _TYPE_OF_FIELD08(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD07(__VA_ARGS__)
#define _TYPE_OF_FIELD07(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD06(__VA_ARGS__)
#define _TYPE_OF_FIELD06(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD05(__VA_ARGS__)
#define _TYPE_OF_FIELD05(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD04(__VA_ARGS__)
#define _TYPE_OF_FIELD04(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD03(__VA_ARGS__)
#define _TYPE_OF_FIELD03(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD02(__VA_ARGS__)
#define _TYPE_OF_FIELD02(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME)), _TYPE_OF_FIELD01(__VA_ARGS__)
#define _TYPE_OF_FIELD01(NAME, ...) IF(HAS_PARENTHESIS(NAME))(_TYPE_OF_FIELD__ NAME, _TYPE_OF_FIELD__(NAME))
#define _TYPE_OF_FIELD__(NAME, ...) {#NAME, offsetof(TYPE, NAME), type_of(t.NAME), "" __VA_ARGS__}

#define _TYPE_OF_NAME(T) IF(HAS_PARENTHESIS(T))(_TYPE_OF_NAME_ T, _TYPE_OF_NAME_(T))
#define _TYPE_OF_NAME_(...) __VA_ARGS__

#define TYPE_OF(T, ...)                                                             \
inline static const Type *                                                          \
type_of(const _TYPE_OF_NAME(T))                                                     \
{                                                                                   \
	static const Type self = {                                                      \
		.name = name_of<_TYPE_OF_NAME(T)>(),                                        \
		.kind = kind_of<_TYPE_OF_NAME(T)>(),                                        \
		.size = sizeof(_TYPE_OF_NAME(T)),                                           \
		.align = alignof(_TYPE_OF_NAME(T)),                                         \
		.as_struct = {}                                                             \
	};                                                                              \
	__VA_OPT__(                                                                     \
		static bool initialized = false;                                            \
		if (initialized)                                                            \
			return &self;                                                           \
		initialized = true;                                                         \
		using TYPE = _TYPE_OF_NAME(T);                                              \
		TYPE t = {};                                                                \
		static const Type_Field fields[] = {OVERLOAD(_TYPE_OF_FIELD, __VA_ARGS__)}; \
		((Type *)&self)->as_struct = {fields, sizeof(fields) / sizeof(Type_Field)}; \
	)                                                                               \
	return &self;                                                                   \
}

TYPE_OF(i8)
TYPE_OF(i16)
TYPE_OF(i32)
TYPE_OF(i64)
TYPE_OF(u8)
TYPE_OF(u16)
TYPE_OF(u32)
TYPE_OF(u64)
TYPE_OF(f32)
TYPE_OF(f64)
TYPE_OF(bool)
TYPE_OF(char)

template <>
inline const Type *
type_of<void>()
{
	static const Type self = {
		.name = name_of<void>(),
		.kind = kind_of<void>(),
		.size = 0,
		.align = 0,
		.as_struct = {}
	};
	return &self;
}

template <>
inline const Type *
type_of<const void>()
{
	static const Type self = {
		.name = name_of<void>(),
		.kind = kind_of<void>(),
		.size = 0,
		.align = 0,
		.as_struct = {}
	};
	return &self;
}

template <typename T>
requires (std::is_pointer_v<T>)
inline static constexpr const Type *
type_of(const T)
{
	static const Type *pointee = nullptr;
	if constexpr (!std::is_abstract_v<std::remove_pointer_t<T>>)
		pointee = type_of<std::remove_pointer_t<T>>();
	static const Type self = {
		.name = name_of<T>(),
		.kind = kind_of<T>(),
		.size = sizeof(T),
		.align = alignof(T),
		.as_pointer = pointee
	};
	return &self;
}

template <typename T, u64 N>
inline static constexpr const Type *
type_of(const T(&)[N])
{
	static const Type self = {
		.name = name_of<T[N]>(),
		.kind = kind_of<T[N]>(),
		.size = sizeof(T[N]),
		.align = alignof(T[N]),
		.as_array = {type_of(T{}), N}
	};
	return &self;
}

#define TYPE_OF_ENUM_VALUE16(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE15(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE15(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE14(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE14(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE13(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE13(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE12(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE12(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE11(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE11(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE10(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE10(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE09(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE09(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE08(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE08(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE07(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE07(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE06(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE06(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE05(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE05(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE04(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE04(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE03(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE03(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE02(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE02(VALUE, ...) {(i32)VALUE, #VALUE}, TYPE_OF_ENUM_VALUE01(__VA_ARGS__)
#define TYPE_OF_ENUM_VALUE01(VALUE, ...) {(i32)VALUE, #VALUE}

#define TYPE_OF_ENUM(T, ...)                                                                           \
inline static const Type *                                                                             \
type_of(const T)                                                                                       \
{                                                                                                      \
	__VA_OPT__(static const Type_Enum_Value values[] = { OVERLOAD(TYPE_OF_ENUM_VALUE, __VA_ARGS__) };) \
	static const Type self = {                                                                         \
		.name = name_of<T>(),                                                                          \
		.kind = kind_of<T>(),                                                                          \
		.size = sizeof(T),                                                                             \
		.align = alignof(T),                                                                           \
		.as_enum = {__VA_OPT__(values, sizeof(values) / sizeof(Type_Enum_Value))}                      \
	};                                                                                                 \
	return &self;                                                                                      \
}

template <typename T>
requires (std::is_enum_v<T>)
inline static constexpr const Type *
type_of(const T)
{
	struct Enum_Value
	{
		i32 index;
		std::string_view name;
	};

	struct Enum
	{
		std::array<Enum_Value, REFLECT_MAX_ENUM_VALUE_COUNT> values;
		u64 count;
	};

	constexpr auto get_enum_value = []<T V>() -> Enum_Value {
		#if defined(_MSC_VER)
			constexpr auto type_function_name      = std::string_view{__FUNCSIG__};
			constexpr auto type_name_prefix_length = type_function_name.find("()<") + 3;
			constexpr auto type_name_length        = type_function_name.find(">", type_name_prefix_length) - type_name_prefix_length;
		#elif defined(__GNUC__) || defined(__clang__)
			constexpr auto type_function_name      = std::string_view{__PRETTY_FUNCTION__};
			constexpr auto type_name_prefix_length = type_function_name.find("= ") + 2;
			constexpr auto type_name_length        = type_function_name.find("]", type_name_prefix_length) - type_name_prefix_length;
		#else
			#error "[REFLECT]: Unsupported compiler."
		#endif

		char c = type_function_name.at(type_name_prefix_length);
		if ((c >= '0' && c <= '9') || c == '(' || c == ')')
			return {};
		return {(i32)V, {type_function_name.data() + type_name_prefix_length, type_name_length}};
	};

	constexpr auto data = [get_enum_value]<i32... I>(std::integer_sequence<i32, I...>) -> Enum {
		return {
			{ get_enum_value.template operator()<(T)(I + REFLECT_MIN_ENUM_VALUE)>()...},
			((get_enum_value.template operator()<(T)(I + REFLECT_MIN_ENUM_VALUE)>().name != "") + ...)
		};
	}(std::make_integer_sequence<i32, REFLECT_MAX_ENUM_VALUE_COUNT>());

	static Type_Enum_Value values[data.count] = {};
	static char names[data.count][REFLECT_MAX_NAME_LENGTH] = {};

	static bool initialized = false;
	if (initialized == false)
	{
		for (u64 i = 0, c = 0; i < REFLECT_MAX_ENUM_VALUE_COUNT; ++i)
		{
			if (const auto &value = data.values[i]; value.name != "")
			{
				values[c].index = value.index;
				::memcpy(names[c], value.name.data(), value.name.length());
				values[c].name = names[c];
				++c;
			}
		}
		initialized = true;
	}

	static const Type self = {
		.name = name_of<T>(),
		.kind = kind_of<T>(),
		.size = sizeof(T),
		.align = alignof(T),
		.as_enum = {values, data.count}
	};
	return &self;
}

template <typename T>
inline static constexpr const Type *
type_of()
{
	return type_of(T{});
}

template <typename T>
inline static constexpr const Value
value_of(T &&type)
{
	T t = type;
	return {&type, type_of(t)};
}

TYPE_OF(Type_Field, name, offset, type)
TYPE_OF(Type, name, kind, size, align, as_struct.fields, as_struct.field_count)
TYPE_OF(Value, data, type)