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
    MACRO(std::unordered_set) SEP			\
    MACRO(std::vector)

// FOR_EACH_STD_CONTAINER_TWO_ARGS(MACRO)
#define FOR_EACH_STD_CONTAINER_TWO_ARGS(MACRO,SEP)	\
    MACRO(std::map) SEP					\
    MACRO(std::unordered_map)

// container constants
namespace container {

constexpr char start = '{';
constexpr char sep = ',';
constexpr char end = '}';
std::string const ellipsis = "...";

template<class T>
void insert(std::vector<T> & v, T&& t) { v.push_back(std::forward<T>(t)); }

template<class T>
void insert(std::set<T> & s, T&& t) { s.insert(std::forward<T>(t)); }

template<class T>
void insert(std::unordered_set<T> & s, T&& t) { s.insert(std::forward<T>(t)); }

} /* namespace Container */
