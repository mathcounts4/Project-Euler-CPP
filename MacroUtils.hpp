#pragma once

// defines useful aggregate macros

#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// FOR_EACH_INTEGRAL_TYPE(MACRO)
#define FOR_EACH_INTEGRAL_TYPE(MACRO,SEP)	\
    MACRO(bool) SEP				\
    MACRO(char) SEP				\
    MACRO(char16_t) SEP				\
    MACRO(char32_t) SEP				\
    MACRO(wchar_t) SEP				\
    MACRO(signed char) SEP			\
    MACRO(unsigned char) SEP			\
    MACRO(short) SEP				\
    MACRO(unsigned short) SEP			\
    MACRO(int) SEP				\
    MACRO(unsigned int) SEP			\
    MACRO(long) SEP				\
    MACRO(unsigned long) SEP			\
    MACRO(long long) SEP			\
    MACRO(unsigned long long)

// FOR_EACH_FLOAT_TYPE(MACRO)
#define FOR_EACH_FLOAT_TYPE(MACRO,SEP)		\
    MACRO(float) SEP				\
    MACRO(double) SEP				\
    MACRO(long double)

// FOR_EACH_PRIMITIVE_TYPE(MACRO)
#define FOR_EACH_PRIMITIVE_TYPE(MACRO,SEP)	\
    FOR_EACH_INTEGRAL_TYPE(MACRO,SEP) SEP	\
    FOR_EACH_FLOAT_TYPE(MACRO,SEP)

// FOR_EACH_STD_CONTAINER_ONE_ARG(MACRO)
#define FOR_EACH_STD_CONTAINER_ONE_ARG(MACRO,SEP)	\
    MACRO(std::list) SEP				\
    MACRO(std::set) SEP					\
    MACRO(std::multiset) SEP				\
    MACRO(std::unordered_set) SEP			\
    MACRO(std::vector)

// FOR_EACH_STD_CONTAINER_TWO_ARGS(MACRO)
#define FOR_EACH_STD_CONTAINER_TWO_ARGS(MACRO,SEP)	\
    MACRO(std::map) SEP					\
    MACRO(std::multimap) SEP				\
    MACRO(std::unordered_map)

// container constants
namespace container {

constexpr char start = '{';
constexpr char sep = ',';
constexpr char end = '}';
std::string const ellipsis = "...";

template<class T>
void insert(std::vector<T>& v, T&& t) { v.push_back(std::forward<T>(t)); }

template<class T>
void insert(std::set<T>& s, T&& t) { s.insert(std::forward<T>(t)); }

template<class T>
void insert(std::unordered_set<T>& s, T&& t) { s.insert(std::forward<T>(t)); }

} /* namespace Container */

#define VAR_STR(var) (#var " = " + ::to_string(var))

#define FE_0(WHAT)
#define FE_1(WHAT, X) WHAT(X) 
#define FE_2(WHAT, X, ...) WHAT(X)FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X)FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X)FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X)FE_4(WHAT, __VA_ARGS__)
#define FE_6(WHAT, X, ...) WHAT(X)FE_5(WHAT, __VA_ARGS__)
#define FE_7(WHAT, X, ...) WHAT(X)FE_6(WHAT, __VA_ARGS__)
#define FE_8(WHAT, X, ...) WHAT(X)FE_7(WHAT, __VA_ARGS__)
#define FE_9(WHAT, X, ...) WHAT(X)FE_8(WHAT, __VA_ARGS__)
#define FE_10(WHAT, X, ...) WHAT(X)FE_9(WHAT, __VA_ARGS__)

#define GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME,...) NAME 
#define FOR_EACH(action, ...)						\
    GET_MACRO(_0,__VA_ARGS__,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0)(action,__VA_ARGS__)
