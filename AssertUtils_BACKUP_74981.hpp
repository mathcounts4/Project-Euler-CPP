#pragma once
<<<<<<< HEAD
#ifndef ASSERT_UTILS
#define ASSERT_UTILS

#include <type_traits>

// defines utility types for compile time static_assert statements


template<class... Args>
struct My_False { static constexpr bool value = false; };

// Macro to ask the user to define a certain class<T> with a certain static inline method
#define error_user_define(type,struct,method)				\
    static_assert(My_False<type>::value,				\
		  "Please define struct "				\
		  #struct "<" #type ">"					\
		  " for your type, with static inline method "		\
=======

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
>>>>>>> main
		  #method)


/* type handling: */

/* determines if a type is a value type or a const lvalue reference */
template<class T>
<<<<<<< HEAD
constexpr bool
is_value_or_const_lref()
{
    return !std::is_reference<T>::value ||
	(std::is_lvalue_reference<T>::value &&
	 std::is_const<typename std::remove_reference<T>::type>::value);
}
=======
constexpr bool is_value_or_const_lref =
    !std::is_reference<T>::value ||
    (std::is_lvalue_reference<T>::value &&
     std::is_const<typename std::remove_reference<T>::type>::value);
>>>>>>> main


/*
My_Enable_If : disables a function with a specified compiler message

Usage:

declare_enable_if(unique_name, message)

template<...>
typename use_enable_if(unique_name, bool_value, return_type)
function_name(...)
*/

<<<<<<< HEAD
template<const int *, bool, class T>
struct My_Enable_If;

#define declare_enable_if(unique_name,message)			\
    constexpr int unique_name = 0;				\
    template<class T>						\
    struct My_Enable_If<&unique_name,true,T>			\
    {								\
	using type = T;						\
    };								\
    template<class T>						\
    struct My_Enable_If<&unique_name,false,T>			\
    {								\
	static_assert(My_False<T>::value,message);		\
    };

#define use_enable_if(unique_name,bool_value,return_type)	\
    My_Enable_If<&unique_name,bool_value,return_type>::type
    


/* boolean operations: any, all */

/* helper for my_any */
namespace
{
    /* template class declaration */
    template<bool... values>
    struct my_any_helper;

    /* no arguments -> any is false */
    template<>
    struct my_any_helper<>
    {
	static constexpr bool value = false;
    };

    /* first argument true -> any is true */
    template<bool... values>
    struct my_any_helper<true,values...>
    {
	static constexpr bool value = true;
    };
    
    /* first argument false -> look at the rest */
    template<bool... values>
    struct my_any_helper<false,values...>
    {
	static constexpr bool value = my_any_helper<values...>::value;
    };
}

/* my_any<true,false,false>() */
template<bool... values>
constexpr bool
my_any()
{
    return my_any_helper<values...>::value;
}

/* helper for my_all */
namespace
{
    /* template class declaration */
    template<bool... values>
    struct my_all_helper;

    /* no arguments -> all is true */
    template<>
    struct my_all_helper<>
    {
	static constexpr bool value = true;
    };

    /* first argument false -> all is false */
    template<bool... values>
    struct my_all_helper<false,values...>
    {
	static constexpr bool value = false;
    };
    
    /* first argument true -> look at the rest */
    template<bool... values>
    struct my_all_helper<true,values...>
    {
	static constexpr bool value = my_all_helper<values...>::value;
    };
}

/* my_all<true,false,true>() */
template<bool... values>
constexpr bool
my_all()
{
    return my_all_helper<values...>::value;
}


#endif /* ASSERT_UTILS */
=======
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
>>>>>>> main
