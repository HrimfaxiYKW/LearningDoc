/*****************************************************************************
函数类型萃取
函数指针提取
 *****************************************************************************/
#pragma once
#include <tuple>
#include "mockcpp/mockcpp.hpp"
// for member function
template <typename T>
class Foo {};

template <typename R, typename C, typename... A>
class Foo<R(C, A...)> {};

template <typename R, typename C, typename... A>
std::tuple<A...> deduce_A_tuple(R (C::*)(A...));

template <typename R, typename C, typename T>
struct FooHelper;

template <typename R, typename C, typename... A>
struct FooHelper<R, C, std::tuple<A...>> {
  typedef Foo<R(C, A...)> type;
};

template <typename>
struct FooTraits;

template <typename>
struct OverloadFooTraits;

// for member function
template <typename R, typename C, typename... A>
struct FooTraits<R (C::*)(A...)> {
  // using type = Foo<R (*)(C *, A...)>;
  using TYPE = R (*)(C *, A...);
};

// for function
template <typename R, typename... A>
struct FooTraits<R(A...)> {
  using TYPE = R (*)(A...);
};

// for overloaded function
#define MOCK_CAT_1(x, y) x##y
#define MOCK_CAT_2(x, y) MOCK_CAT_1(x, y)
#define __mock_unique_varible_name MOCK_CAT_2(_overload_func, __LINE__)

// for member overloaded function
template <typename R, typename C, typename... A>
struct OverloadFooTraits<R(C *, A...)> {
  typedef R (C::*FunctionPtr)(A...);
};

// for global overloaded function
template <typename R, typename... A>
struct OverloadFooTraits<R(A...)> {
  typedef R (*FunctionPtr)(A...);
};

#define MOCK_SIMPLE_FUNCTION(func) MOCKCPP_NS::mockAPI(#func, reinterpret_cast<FooTraits<decltype(func)>::TYPE>(func))

#define MOCK_OVERLOAD_FUNCTION(func, sign)                         \
  OverloadFooTraits<sign>::FunctionPtr __mock_unique_varible_name; \
  __mock_unique_varible_name = &func;                              \
  MOCK_SIMPLE_FUNCTION(__mock_unique_varible_name)

// #define GET_MACRO(_1, _2, NAME, ...) NAME
// #define MOCK_FUNCTION(...) GET_MARCRO(#__VA_ARGS__, MOCK_OVERLOAD_FUNCTION, MOCK_SIMPLE_FUNCTION)(__VA_ARGS__)
// get number of arguments with __NARG__
#define __NARG__(...) __NARG_I_(__VA_ARGS__, __RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, \
                _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42,  \
                _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62,  \
                _63, N, ...)                                                                                         \
  N
#define __RSEQ_N()                                                                                                    \
  63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, \
      34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, \
      5, 4, 3, 2, 1, 0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__))(__VA_ARGS__)

// definition for MOCK_FUNCTION
#define MOCK_FUNCTION(...) VFUNC(MOCK_FUNCTION, __VA_ARGS__)
#define MOCK_FUNCTION1(func) MOCK_SIMPLE_FUNCTION(func)
#define MOCK_FUNCTION2(func, sign) MOCK_OVERLOAD_FUNCTION(func, sign)
