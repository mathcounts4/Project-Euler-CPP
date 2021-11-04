#pragma once

#include "TypeUtils.hpp"

#include <functional>
#include <type_traits>
#include <utility>

// defines utility types for compile time static_assert statements

template<class... Args>
constexpr bool False = false;

template<template<class...> class Template>
constexpr bool False_T = false;

// Macro to tell the programmer to define a certain struct with a certain static inline method
#define error_user_define(struct,method)	\
    static_assert(False<struct>,				\
		  "Please define struct "			\
		  #struct					\
		  " for your type, with static inline method "	\
		  #method)


/* type handling: */

/* determines if a type is a value type or a const lvalue reference */
template<class T>
constexpr bool is_value_or_const_lref =
    !std::is_reference<T>::value ||
    (std::is_lvalue_reference<T>::value &&
     std::is_const<typename std::remove_reference<T>::type>::value);


/*
My_Enable_If : disables a function with a specified compiler message

Usage:

declare_enable_if(unique_name, message)

template<...>
typename use_enable_if(unique_name, bool_value, return_type)
function_name(...)
*/

#define declare_enable_if(unique_name,message)			\
    template<bool b, class T> struct unique_name;		\
    template<class T> struct unique_name<true,T>		\
    { using type = T; };					\
    template<class T> struct unique_name<false,T>		\
    { static_assert(False<unique_name>,message); }

template<template<bool,class> class unique_name, bool b, class T>
using enable_if = typename unique_name<b,T>::type;

template<class T, class Hash = std::hash<T> >
constexpr bool is_default_hashable = std::__check_hash_requirements<T,Hash>::value;


/* Error<Args...> - instantiating causes a compiler error */
template<class... Args>
struct Error { static_assert(False<Args...>,"Intentional assert."); };

/* Invalid_Type<T> - instantiating causes a compiler error */
template<class T>
struct Invalid_Type { static_assert(False<T>,"Error: invalid type"); };

/* assert_same_type<X,Y>() errors if X and Y are different, displaying X and Y in the error message */
template<class X, class Y> constexpr void assert_same_type() { static_assert(is_same<X,Y>,"Expected same type"); }

/* assert_same_value<x,y>() errors if x and y are different, displaying x and y in the error message */
template<auto x, auto y> constexpr void assert_same_value() { static_assert(is_same_value<x,y>,"Expected same value"); }
