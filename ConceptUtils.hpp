#pragma once

/*

Defines templates to help implement
a weak form of concepts.

*/

#include "TypeUtils.hpp"

namespace detail
{
    template<template<class...> class C, class v, class... Args>
    constexpr B supports_h = false;

    template<template<class...> class C, class... Args>
    constexpr B supports_h<C,
			   std::void_t<C<Args...> >,
			   Args...> = true;
}

template<template<class...> class C, class... Args>
constexpr B supports = detail::supports_h<C,void,Args...>;



namespace detail
{
    template<B b,
	     template<class...> class X,
	     class X_Tuple,
	     template<class...> class Y,
	     class Y_Tuple>
    struct Cond_T_S;

    template<template<class...> class X,
	     class... X_T,
	     template<class...> class Y,
	     class... Y_T>
    struct Cond_T_S<true,X,std::tuple<X_T...>,Y,std::tuple<Y_T...> >
    { using Type = X<X_T...>; };
    
    template<template<class...> class X,
	     class... X_T,
	     template<class...> class Y,
	     class... Y_T>
    struct Cond_T_S<false,X,std::tuple<X_T...>,Y,std::tuple<Y_T...> >
    { using Type = Y<Y_T...>; };
}

template<B b,
	 template<class...> class X,
	 class X_T,
	 template<class...> class Y,
	 class Y_T>
using Cond_T = Type<detail::Cond_T_S<b,X,X_T,Y,Y_T> >;




/* Defines if the given type is constructible with the given arguments */
namespace detail
{
    template<class T, class... Args>
    constexpr auto is_constructible_function()
	-> typename std::enable_if<std::is_convertible_v<decltype(T(declval<Args&&>...)),T>,B>
    { return true; }
    
    template<class T, class... Args>
    constexpr B is_constructible_function() { return false; }
}
template<class T, class... Args>
constexpr B is_constructible = detail::is_constructible_function<T,Args...>();
