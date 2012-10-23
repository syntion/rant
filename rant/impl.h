#pragma once

// Copyright (c) 2012, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the GPLv2 or newer

#include "rant/util.h"
#include "rant/check.h"

#define RANT_CLASS_NAME range
#define RANT_VALUE      __val

#define ADD_QUOTES(STR) #STR
#define STRINGIZE(STR) ADD_QUOTES(STR)

#define RANT_ARITHMETIC_OPS                                        \
	RANT_OPERATOR_ASSIGNMENT(+)                                    \
	RANT_OPERATOR_ASSIGNMENT(-)                                    \
	RANT_OPERATOR_ASSIGNMENT(*)                                    \
	RANT_OPERATOR_ASSIGNMENT(/)                                    \
	RANT_OPERATOR_UNARY_RET(type, +)                               \
	RANT_OPERATOR_UNARY_RET(type, -)

#define RANT_DEFAULT_BODY(CLASS_NAME)                              \
protected:                                                         \
	T RANT_VALUE;                                                  \
public:                                                            \
	typedef CLASS_NAME <T, Max, Min, Check, void> type;            \
                                                                   \
	constexpr CLASS_NAME (T v = T()) : RANT_VALUE(Check(v))        \
	{                                                              \
	}                                                              \
                                                                   \
	inline explicit operator T () const noexcept                   \
	{                                                              \
		return RANT_VALUE;                                         \
	}                                                              \
                                                                   \
	RANT_ARITHMETIC_OPS                                            \

#if __cplusplus != 201103L
#define RANT_STATIC_ASSERT
#else
#define RANT_STATIC_ASSERT                                         \
	static_assert(                                                 \
		value<T, Max>() >= value<T, Min>(), "Max must be >= Min");
#endif

#define RANT_OPERATOR_UNARY_RET(RET, OP)                           \
	inline RET operator OP () const                                \
	{                                                              \
		return OP RANT_VALUE;                                      \
	}

#define RANT_OPERATOR_ASSIGNMENT(OP)                               \
	inline type& operator OP##= (T x)                              \
	{                                                              \
		RANT_VALUE = Check(RANT_VALUE OP x);                       \
		return *this;                                              \
	}                                                              \
	inline type& operator OP##= (type x)                           \
	{                                                              \
		return operator OP##=(static_cast<T>(x));                  \
	}

#define RANT_OPERATOR_INCREMENTAL(OP)                              \
	inline type& operator OP##OP ()                                \
	{                                                              \
		return *this OP##= 1;                                      \
	}                                                              \
	inline type operator OP##OP (int)                              \
	{                                                              \
		type t(*this);                                             \
		*this OP##= 1;                                             \
		return t;                                                  \
	}

#define RANT_OPERATOR_BINARY_FF_RET(CLASS_NAME, RET, OP)           \
	template<typename T, typename Max, typename Min, T(*Check)(T)> \
	inline RET operator OP (                                       \
		CLASS_NAME<T, Max, Min, Check> a,                          \
		CLASS_NAME<T, Max, Min, Check> b) noexcept                 \
	{                                                              \
		return static_cast<T>(a) OP static_cast<T>(b);             \
	}                                                              \
	template<typename U, typename T,                               \
		typename Max, typename Min, T(*Check)(T)>                  \
	inline typename                                                \
	std::enable_if<std::is_arithmetic<U>::value, RET>::type        \
	operator OP (                                                  \
		CLASS_NAME<T, Max, Min, Check> a,                          \
		U b) noexcept                                              \
	{                                                              \
		return static_cast<T>(a) OP b;                             \
	}                                                              \
	template<typename U, typename T,                               \
		typename Max, typename Min, T(*Check)(T)>                  \
	inline typename                                                \
	std::enable_if<std::is_arithmetic<U>::value, RET>::type        \
	operator OP (                                                  \
		U a,                                                       \
		CLASS_NAME<T, Max, Min, Check> b) noexcept                 \
	{                                                              \
		return a OP static_cast<T>(b);                             \
	}

#define RANT_FWD(CLASS_NAME)                                       \
	template<typename T, typename Max, typename Min,               \
		T(*Check)(T) = throw_on_error<T, Max, Min>,                \
		typename Enable = void>                                    \
	class CLASS_NAME;

#define RANT_IMPL(TYPE, CLASS_NAME, ...)                           \
	template<typename T, typename Max, typename Min, T(*Check)(T)> \
	class CLASS_NAME<T, Max, Min, Check, typename                  \
		std::enable_if<std::is_##TYPE <T>::value>::type>           \
	{                                                              \
		static_assert(std::is_##TYPE <T>::value,                   \
					  "T must be " STRINGIZE(TYPE) " type");       \
		RANT_DEFAULT_BODY(CLASS_NAME)                              \
		RANT_STATIC_ASSERT                                         \
	public:                                                        \
		__VA_ARGS__                                                \
	};


namespace rant {

RANT_FWD(RANT_CLASS_NAME)

RANT_IMPL(floating_point, RANT_CLASS_NAME,
	static_assert(is_ratio<Max>::value,
				  "Max must be std::ratio type");
	static_assert(is_ratio<Min>::value,
				  "Min must be std::ratio type");
)

RANT_IMPL(integral, RANT_CLASS_NAME,
	static_assert(is_integral_constant<Max>::value,
				  "Max must be std::integral_constant type");
	static_assert(is_integral_constant<Min>::value,
				  "Min must be std::integral_constant type");


	RANT_OPERATOR_ASSIGNMENT(%)
	RANT_OPERATOR_INCREMENTAL(+)
	RANT_OPERATOR_INCREMENTAL(-)

	RANT_OPERATOR_UNARY_RET(bool, !)
	RANT_OPERATOR_UNARY_RET(type, ~)

	RANT_OPERATOR_ASSIGNMENT(&)
	RANT_OPERATOR_ASSIGNMENT(|)
	RANT_OPERATOR_ASSIGNMENT(^)
	RANT_OPERATOR_ASSIGNMENT(<<)
	RANT_OPERATOR_ASSIGNMENT(>>)
)

#define RANT_RETURN_TYPE RANT_CLASS_NAME<T, Max, Min, Check>
#define RANT_RETURN_TYPE_INT \
	typename std::enable_if<std::is_integral<T>::value, RANT_CLASS_NAME<T, Max, Min, Check>>::type
#define RANT_RETURN_TYPE_INT_BOOL \
	typename std::enable_if<std::is_integral<T>::value, bool>::type

RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE, +)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE, -)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE, *)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE, /)

RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, %)

RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, &)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, |)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, ^)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, <<)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT, >>)

RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT_BOOL, &&)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, RANT_RETURN_TYPE_INT_BOOL, ||)

RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, ==)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, !=)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, <)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, >)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, <=)
RANT_OPERATOR_BINARY_FF_RET(RANT_CLASS_NAME, bool, >=)

template<typename T, typename Max, typename Min, T(*Check)(T), typename Enable>
struct numeric_limits<range<T, Max, Min, Check, Enable>>
{
	static constexpr T max() { return value<T, Max>(); }
	static constexpr T lowest() { return value<T, Min>(); }
};

} // namespace rant

#undef RANT_VALUE
#undef ADD_QUOTES
#undef STRINGIZE
#undef RANT_ARITHMETIC_OPS
#undef RANT_DEFAULT_BODY
#undef RANT_STATIC_ASSERT
#undef RANT_OPERATOR_UNARY_RET
#undef RANT_OPERATOR_ASSIGNMENT
#undef RANT_OPERATOR_INCREMENTAL
#undef RANT_OPERATOR_BINARY_FF_RET
#undef RANT_FWD
#undef RANT_IMPL
#undef RANT_RETURN_TYPE
#undef RANT_RETURN_TYPE_INT
#undef RANT_RETURN_TYPE_INT_BOOL
