#pragma once

#include <tuple>

#include "AssertUtils.hpp"
#include "Tuple.hpp"
#include "TypeUtils.hpp"

// tuple_size<Tuple>
template<class Tuple>
constexpr SZ tuple_size = std::tuple_size_v<No_cvref<Tuple> >;

// Tuple_Indices<Tuple>
template<class Tuple>
using Tuple_Indices = std::make_index_sequence<tuple_size<Tuple> >;

// Tuple_Element<index,Tuple>
template<SZ index, class Tuple>
using Tuple_Element = std::tuple_element_t<index,No_cvref<Tuple> >;

// is_tuple<Type>
namespace detail
{
    template<class>
    constexpr B is_tuple_plain = false;
    template<class... Args>
    constexpr B is_tuple_plain<std::tuple<Args...> > = true;
}
template<class T>
constexpr B is_tuple = detail::is_tuple_plain<No_cvref<T> >;

// As_Tuple<T>
// Adds a tuple wrapper around the argument if not already a tuple
template<class T>
using As_Tuple = Cond<is_tuple<T>,T,std::tuple<T> >;

/* As_Ref_Tuple or FWD_T is the type of a tuple of forwarding references */
namespace detail
{
    template<class Tuple, class Index_Sequence = Tuple_Indices<Tuple> >
    struct As_Ref_Tuple_S;

    template<class Tuple, SZ... I>
    struct As_Ref_Tuple_S<Tuple,Indices<I...> >
    { using Type = std::tuple<As_ref<Tuple_Element<I,Tuple> >...>; };
}
template<class T>
using As_Ref_Tuple = Type<detail::As_Ref_Tuple_S<As_Tuple<T> > >;
template<class T>
using FWD_T = As_Ref_Tuple<T>;


namespace detail
{
    /* struct helper for elements_satisfy */
    template<template<B...> class A, class T, template<class> class C, class Index_Seq = Tuple_Indices<T> >
    struct elements_satisfy_S;

    template<template<B...> class A, class T, template<class> class C, SZ... I>
    struct elements_satisfy_S<A,T,C,Indices<I...> >
    { static constexpr B value = A<C<Tuple_Element<I,T> >::value...>::value; };
}

/* elements_satisfy<application A, tuple T, condition C>
   returns A<C<E1>,C<E2>,...> where E# are the elements of T */
template<template<B...> class A, class T, template<class> class C>
constexpr B elements_satisfy = detail::elements_satisfy_S<A,As_Tuple<T>,C>::value;

/* any_element<T,C> indicates if C holds for any tuple element of T
template<class T, template<class> class C>
constexpr B any_element = elements_satisfy<any_S,T,C>;

 all_elements<T,C> indicates if C holds for all tuple elements of T
template<class T, template<class> class C>
constexpr B all_elements = elements_satisfy<all_S,T,C>;

 no_elements<T,C> indicates if C holds for no tuple elements of T
template<class T, template<class> class C>
constexpr B no_elements = elements_satisfy<none_S,T,C>;
*/

// Tuple_Cat<Args...>
// Concatenated input tuple types into a big tuple,
// expanading tuples by one layer:
// {{A,B},C}, D, {E} -> {{A,B},C,D,E}
namespace detail
{
    template<class... Tuples>
    struct Tuple_Cat_S;

    template<>
    struct Tuple_Cat_S<>
    { using Type = std::tuple<>; };

    template<class Tuple>
    struct Tuple_Cat_S<Tuple>
    { using Type = Tuple; };

    template<class... Args1, class... Args2, class... Tuples>
    struct Tuple_Cat_S<std::tuple<Args1...>,std::tuple<Args2...>,Tuples...>
    { using Type = Type<Tuple_Cat_S<std::tuple<Args1...,Args2...>,Tuples...> >; };
}
template<class... Tuples>
using Tuple_Cat = Type<detail::Tuple_Cat_S<Tuples...> >;

template<class... Args>
using Cat_Into_Tuple = Tuple_Cat<As_Tuple<Args>...>;

// If already a tuple, returns the input argument (as a reference)
// If not a tuple, returns a tuple containing the input as a reference
template<class Arg>
constexpr As_Tuple<Arg&&> as_tuple(Arg&& arg) { return fwd<Arg>(arg); }

// flattens tuples in the input by 1 layer
// and returns a tuple of all input arguments, as references
template<class... Args>
constexpr decltype(auto) forward_as_tuple_of_refs(Args&&... args) { return std::tuple_cat(as_tuple(fwd<Args>(args))...); }

// No_cvref_Tuple<Args...> - tuple of Args without cv and references
template<class... Types>
using No_cvref_Tuple = std::tuple<No_cvref<Types>...>;

// Take_After
namespace detail
{
    template<SZ N, class Tuple>
    struct Take_After_S;
    
    template<>
    struct Take_After_S<0,std::tuple<> >
    {
	using Type = std::tuple<>;
    };
    
    template<SZ N, class First, class... Rest>
    struct Take_After_S<N,std::tuple<First,Rest...> >
    {
	using Type = Type<Take_After_S<N-1,std::tuple<Rest...> > >;
    };

    template<class First, class... Rest>
    struct Take_After_S<0,std::tuple<First,Rest...> >
    {
	using Type = std::tuple<First,Rest...>;
    };
}
template<SZ N, class Tuple>
using Take_After = Type<detail::Take_After_S<N,Tuple> >;

// Take_First
namespace detail
{
    template<class Tuple, class Index_Seq>
    struct Take_First_S;
    
    template<class Tuple, SZ... I>
    struct Take_First_S<Tuple,Indices<I...> >
    {
	using Type = std::tuple<Tuple_Element<I,Tuple>...>;
    };
}
template<SZ N, class Tuple>
using Take_First = Type<detail::Take_First_S<Tuple,std::make_index_sequence<N> > >;

namespace detail
{
    template<B... matching>
    struct First_Argument_Satisfied
    {
	template<class... Args>
	constexpr void operator()(Args&&...) {}
    };

    template<B... rest>
    struct First_Argument_Satisfied<true,rest...>
    {
	template<class T, class... Args>
	constexpr decltype(auto) operator()(T&& t, Args&&...) { return fwd<T>(t); }
    };
    
    template<B... rest>
    struct First_Argument_Satisfied<false,rest...>
    {
	template<class T, class... Args>
	constexpr decltype(auto) operator()(T&&, Args&&... args)
	    { return First_Argument_Satisfied<rest...>{}(fwd<Args>(args)...); }
    };
}

// unique_argument_matching<Desired>(args...) -> Desired
template<class Desired, class... Args>
constexpr decltype(auto) unique_argument_matching(Args&&... args) {
    static_assert(is_pure<Desired>,"Desired argument should not be cv-ref qualified");
    constexpr SZ num_matches = sum<is_same<Desired,No_cvref<Args> >...>;
    static_assert(num_matches > 0,"No matches");
    static_assert(num_matches <= 1,"Too many matches");

    return detail::First_Argument_Satisfied<is_same<Desired,No_cvref<Args> >...>{}(fwd<Args>(args)...);
}

namespace detail
{
    
    template<B selected>
    struct Argument_Or_Empty;
    
    template<> struct Argument_Or_Empty<true>
    { template<class T> static constexpr std::tuple<T&&> select(T&& t) { return fwd<T>(t); } };
    
    template<> struct Argument_Or_Empty<false>
    { template<class T> static constexpr std::tuple<> select(T&&) { return {}; } };
    
    template<B... selected>
    struct Select_Arguments
    { template<class... Args> constexpr decltype(auto) operator()(Args&&... args) const {
	return std::tuple_cat(Argument_Or_Empty<selected>::select(fwd<Args>(args))...); } };
}

// all_arguments_matching<Desired>(args...) -> tuple<Desired,Desired,...>
template<class Desired, class... Args>
constexpr decltype(auto) all_arguments_matching(Args&&... args) {
    static_assert(is_pure<Desired>,"Desired argument should not be cv-ref qualified");
    return detail::Select_Arguments<is_same<Desired,No_cvref<Args> >...>{}(fwd<Args>(args)...);
}

// all_arguments_not_matching<Desired>(args...) -> tuple<X,Y,...>
template<class Desired, class... Args>
constexpr decltype(auto) all_arguments_not_matching(Args&&... args) {
    static_assert(is_pure<Desired>,"Desired argument should not be cv-ref qualified");
    return detail::Select_Arguments<!is_same<Desired,No_cvref<Args> >...>{}(fwd<Args>(args)...);
}
