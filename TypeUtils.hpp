#pragma once

/* TODO: ensure "using" is used as often as possible (and we minimize type instantiations */

#include <type_traits>
#include <utility>

/* Easy type names */
using B = bool;
using C = char;
using UC = unsigned char;
using SC = signed char;
using US = unsigned short;
using SS = signed short;
using UI = unsigned int;
using SI = signed int;
using UL = unsigned long;
using SL = signed long;
using ULL = unsigned long long;
using SLL = signed long long;
using U128 = __uint128_t;
using S128 = __int128_t;
using SZ = std::size_t;
using F = float;
using D = double;
using LD = long double;
using Argc = SI;
using Argv = C const * const *;

/* boolean operations: any, all, none */

/* any<true,false,false> */
template<B... values>
constexpr B any = (false || ... || values);

/* all<true,false,true> */
template<B... values>
constexpr B all = (true && ... && values);

/* none<true,false,false> */
template<B... values>
constexpr B none = (true && ... && !values);

/* sum<1,3,4> */
template<auto... values>
static constexpr auto sum = (values + ...);
template<>
static constexpr SZ sum<> = 0;

// declval
template<class T>
T&& declval();

// Type<T> = T::type
template<class T>
using Type = typename T::Type;

namespace detail {
    template<B> struct Cond_S;
    template<> struct Cond_S<true> {
	template<class T, class> using Type = T;
    };
    template<> struct Cond_S<false> {
	template<class, class F> using Type = F;
    };
}
// if b has type T else has type F
template<B b, class T, class F>
using Cond = typename detail::Cond_S<b>::template Type<T,F>;

/* remove const */
template<class T>
using No_c = std::remove_const_t<T>;

/* remove volatile */
template<class T>
using No_v = std::remove_volatile_t<T>;

/* remove cv */
template<class T>
using No_cv = No_c<No_v<T> >;

/* remove references (lvalue and rvalue) */
template<class T>
using No_ref = std::remove_reference_t<T>;

/* remove cv and ref */
template<class T>
using No_cvref = No_cv<No_ref<T> >;

/* fwd<T>(lvalue_reference) */
template<class T, class Arg>
constexpr T&& fwd(Arg&& t) noexcept { return std::forward<T>(t); }

/* move(x) */
template<class T>
constexpr No_ref<T>&& move(T&& t) noexcept { return std::move(t); }

/* identity(x) */
template<class T>
constexpr T&& identity(T&& t) noexcept { return fwd<T>(t); }

/* verify */
template<B b>
using verify = std::enable_if_t<b>;

/* make unsigned */
template<class T>
using Make_u = std::make_unsigned_t<T>;
namespace std {
    template<> struct make_unsigned<F> { using type = F; };
    template<> struct make_unsigned<D> { using type = D; };
    template<> struct make_unsigned<LD> { using type = LD; };
    template<> struct make_unsigned<__int128_t> { using type = __uint128_t; };
    template<> struct make_unsigned<__uint128_t> { using type = __uint128_t; };
}

/* common type between two inputs */
template<class X, class Y>
using Common = std::common_type_t<X,Y>;

/* common unsigned type between two inputs */
template<class X, class Y>
using Common_u = Common<Make_u<X>,Make_u<Y> >;

/* is_a< vector , vector<int const> > = true */
/* is_a< vector , vector<int> const > = false */
/* is_a< vector , set<char> > = false */
/* is_a< vector , int > = false */
template<template<class... Args> class Template, class Test>
static constexpr B is_a = false;
template<template<class... Args> class Template, class... Args>
static constexpr B is_a<Template,Template<Args...> > = true;

/* From<X>, To<Y>, and From_To<From<X>,To<Y> > definitions */
template<class X> struct From {
    using Type = X;
};
template<class Y> struct To {
    using Type = Y;
};
template<class X, class Y>
struct From_To {
  private:
    static constexpr B from_X = is_a<From,X>;
    static constexpr B to_Y = is_a<To,Y>;
    static_assert(from_X,"Wrap first argument X in From<X>");
    static_assert(to_Y,"Wrap second argument Y in To<Y>");
  public:
    using Fixed_X = Type<Cond<from_X,X,From<X> > >;
    using Fixed_Y = Type<Cond<to_Y,Y,To<Y> > >;
};

/* FT_F (base from) and FT_T (base to) for From,To pairs */
template<class X, class Y>
using FT_F = typename From_To<X,Y>::Fixed_X;
template<class X, class Y>
using FT_T = typename From_To<X,Y>::Fixed_Y;

/* is_same<X,Y> */
template<class X, class Y>
static constexpr B is_same = false;
template<class X>
static constexpr B is_same<X,X> = true;
template<class X, class... Y> constexpr B any_same = any<is_same<X,Y>...>;

/* is_same_T<TX,TY> */
template<template<class...> class X,
	 template<class...> class Y>
static constexpr B is_same_T = false;
template<template<class...> class X>
static constexpr B is_same_T<X,X> = true;

/* is_same_value<x,y> */
template<auto x, auto y> static constexpr B is_same_value = x == y;

/* is_U<T> */
template<class T>
static constexpr B is_U = is_same<T,Make_u<T> >;

namespace detail {
    /* Add_S, Rem_S, and Copy_S helper declarations */
    template<class T, C... types>
    struct Add_S;
    template<class T, C... types>
    struct Rem_S;
    template<class X, class Y, C... types>
    struct Copy_S;
} /* namespace detail */

/* Add, Rem, and Copy declarations */
template<class T, C... types>
using Add = Type<detail::Add_S<T,types...> >;
template<class T, C... types>
using Rem = Type<detail::Rem_S<T,types...> >;
template<class X, class Y, C... types>
using Copy = Type<detail::Copy_S<X,Y,types...> >;

/* is<T,attribute> */
template<class T, C attribute>
constexpr B is = !is_same<T,Rem<T,attribute> >;

/* is_all<T,attributes...> */
template<class T, C... attributes>
constexpr B is_all = all<is<T,attributes>...>;

/* is_any<T,attributes...> */
template<class T, C... attributes>
constexpr B is_any = any<is<T,attributes>...>;

/* is_none<T,attributes...> */
template<class T, C... attributes>
constexpr B is_none = none<is<T,attributes>...>;

/* is_pure<T> */
template<class T>
constexpr B is_pure = is_none<T,'c','v','r','l'>;

/* is_obj<T> */
template<class T>
constexpr B is_obj = is_none<T,'r','l'>;

/* Copy cvref from X to Y (doesn't remove Y's cvref) */
template<class X, class Y>
using Copy_cvref = Copy<X,Y,'c','v','r','l'>;

/* As_ref<T> adds & if T is not already a reference type */
template<class T>
using As_ref = Cond<is_any<T,'l','r'>,T,Add<T,'l'> >;

/* inherits<From<X>,To<Y> > */
template<class X, class Y>
constexpr B inherits = std::is_convertible_v<FT_T<X,Y>*,FT_F<X,Y>*>;

namespace detail {
    /* Add_S details */
    template<class T>
    struct Add_S<T> { using Type = T; };
    template<class T, C t1, C... types>
    struct Add_S<T,t1,types...> { using Type = Add<Add<T,t1>,types...>; };
    template<class T, C type>
    struct Add_S<T,type> {
	static_assert(!is_same<T,T>, "Must be used with c (const), v (volatile), l (l-value reference), or r (r-value reference)");
    };

    /* c, v, l, r definitions for Add_S */
    template<class T>
    struct Add_S<T,'l'> { using Type = T&; };
    template<class T>
    struct Add_S<T,'r'> { using Type = T&&; };
    template<class T>
    struct Add_S<T,'c'> { using Type = T const; };
    template<class T>
    struct Add_S<T,'v'> { using Type = T volatile; };

    /* Rem_S details */
    template<class T>
    struct Rem_S<T> { using Type = T; };
    template<class T, C t1, C... types>
    struct Rem_S<T,t1,types...> { using Type = Rem<Rem<T,t1>,types...>; };
    
    /* c, v, l, r definitions for Rem_S */
    template<class T> struct Rem_S<T ,'l'> { using Type = T; };
    template<class T> struct Rem_S<T&,'l'> { using Type = T; };
    template<class T> struct Rem_S<T  ,'r'> { using Type = T; };
    template<class T> struct Rem_S<T&&,'r'> { using Type = T; };
    template<class T> struct Rem_S<T        ,'c'> { using Type = T; };
    template<class T> struct Rem_S<T const  ,'c'> { using Type = T; };
    template<class T> struct Rem_S<T const& ,'c'> { using Type = T&; };
    template<class T> struct Rem_S<T const&&,'c'> { using Type = T&&; };
    template<class T> struct Rem_S<T           ,'v'> { using Type = T; };
    template<class T> struct Rem_S<T volatile  ,'v'> { using Type = T; };
    template<class T> struct Rem_S<T volatile& ,'v'> { using Type = T&; };
    template<class T> struct Rem_S<T volatile&&,'v'> { using Type = T&&; };
    
    
    /* Copy_S details */
    template<class X, class Y>
    struct Copy_S<X,Y>
	: From_To<X,Y> {
	using Type = FT_T<X,Y>;
    };
    
    template<class X, class Y, C t1, C... types>
    struct Copy_S<X,Y,t1,types...> : From_To<X,Y> {
	using Type = Copy<X,
			  To<Cond<is<FT_F<X,Y>,t1>,
				  Add<FT_T<X,Y>,t1>,
				  FT_T<X,Y> > >,
			  types...>;
    };
} /* namespace detail */
