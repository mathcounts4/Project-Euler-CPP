#pragma once

#include <type_traits>

#include "AssertUtils.hpp"
#include "ConceptUtils.hpp"
#include "TypeUtils.hpp"

namespace algorithm
{
    template<template<class...> class Type, class Functor>
    struct Hook
    {
	template<class... Args>
	static constexpr B can_invoke =
	    std::is_invocable_v<Functor&&,Args...>;

	// If functor is a templated version of the Type,
	// this is a default hook,
	// so we need to store the functor ourselves
	Cond<is_a<Type,Functor>,Functor,Functor&> functor;
	
	template<class... Args>
        decltype(auto) operator()(Args&& ... args)
	    { return functor(fwd<Args>(args)...); }
	
	template<class... Args>
        decltype(auto) operator()(Args&& ... args) const
	    { return std::as_const(functor)(fwd<Args>(args)...); }
    };
    

    // "hook" constructs a Hook where only the hook type must
    // be specified as a template argument
    template<template<class...> class Type, class Functor>
    Hook<Type,Functor> hook(Functor&& functor) { return {functor}; }
    
    namespace detail
    {
	template<class Hook_Wrapper, template<class...> class Type>
	constexpr B hook_matches = false;

	template<template<class...> class Type,
		 class Functor>
	constexpr B hook_matches<Hook<Type,Functor>,Type> = true;
    }
	
    template<template<class...> class... Hooks> struct Needed;
    template<class... Hooks> struct Used;
    template<class... Types> struct Base;
    template<class> class Empty {};
    struct Ignore
    {
	template<class... Args>
	inline void operator()(Args&& ...) const {}
    };
    template<auto value>
    struct Constant
    {
	template<class... Args>
	inline auto operator()(Args&& ...) const { return value; }
    };
    struct Identity
    {
	template<class T>
	inline T&& operator()(T&& t) const { return fwd<T>(t); }
    };
    template<template<class...> class Hook_Type, class Return>
    struct No_Default
    {
	template<class... Args>
	inline Return operator()(Args&& ...) const
	    {
		static_assert(False_T<Hook_Type>,
			      "Attempted default use of a hook without a default");
	    }
    };
    struct Get {};
    struct Set {};
    template<class Hook_T, template<class...> class... Types>
    using Ignore_Matching =
	Cond<any<detail::hook_matches<Hook_T,Types>...>,
	     Hook<Empty,Hook_T>,
	     Hook_T>;
    template<class Hook_T, template<class...> class... Types>
    using Select_Matching =
	Cond<any<detail::hook_matches<Hook_T,Types>...>,
	     Hook_T,
	     Hook<Empty,Hook_T> >;
    template<class T> struct Hook<Empty,T>
    { template<class... Args> Hook(Args&&...) {} };
    
    namespace detail
    {
        template<template<class...> class Need, class Default_Hook, class... Hooks>
	struct First_Match_Or_Default_Hook_S;
	template<template<class...> class Need, class Default_Hook>
	struct First_Match_Or_Default_Hook_S<Need,Default_Hook>
	{ using Type = Default_Hook; };
	template<template<class...> class Need, class Default_Hook, class Bad, class... Hooks>
	struct First_Match_Or_Default_Hook_S<Need,Default_Hook,Bad,Hooks...>
	{ using Type = Type<First_Match_Or_Default_Hook_S<Need,Default_Hook,Hooks...> >; };
	template<template<class...> class Need, class Default_Hook, class Functor, class... Hooks>
	struct First_Match_Or_Default_Hook_S<Need,Default_Hook,Hook<Need,Functor>,Hooks...>
	{ using Type = Hook<Need,Functor>; };

	template<template<class...> class Need,
		 class Default_Hook,
		 class Used>
	struct Match_Or_Default_Hook_S
	{
	    static_assert(False<Match_Or_Default_Hook_S<Need,Default_Hook,Used> >,
			  "Third template argument to Match_Or_Default must be Used<...>");
	};
	template<template<class...> class Need,
		 class Default_Hook,
		 class... Used_Hooks>
	struct Match_Or_Default_Hook_S<Need,Default_Hook,Used<Used_Hooks...> >
	{
	    using Type = Type<First_Match_Or_Default_Hook_S<Need,Default_Hook,Used_Hooks...> >;
	    static constexpr SZ matching_types = (0 + ... + hook_matches<Used_Hooks,Need>);
	    static_assert(matching_types < 2,
			  "Duplicate type found when extracting template type");
	};

	template<class Have, template<class...> class... Possible>
	struct Cleanup_S
	{
	    static constexpr B is_empty = hook_matches<Have,Empty>;
	    static constexpr B matches_any = (false || ... || hook_matches<Have,Possible>);
	    static constexpr B valid = is_empty || matches_any;
	    using Type = Cond<matches_any,Empty<Have>,Have>;
	    static_assert(valid,"Class did not match against any possible types");
	};
    }

    template<template<class...> class Need, class Used_Hooks>
    using Match = Type<detail::Match_Or_Default_Hook_S<Need,No_Default<Need,void>,Used_Hooks> >;
    
    template<class N, class U, class B> struct Validate
    {
	static_assert(False<Validate<N,U,B> >,
		      "Template arguments to Validate must be Needed<...>, Used<...>, Base<...>");
    };
    
    template<template<class...> class... Needed_Templates,
	     class... Used_Hooks,
	     class... Base_Types>
    struct Validate<Needed<Needed_Templates...>,
		    Used<Used_Hooks...>,
		    Base<Base_Types...> >
	:
        Type<detail::Match_Or_Default_Hook_S<Needed_Templates,
					     Hook<Needed_Templates,Needed_Templates<Base_Types...> >,
					     Used<Used_Hooks...> > >...,
	Type<detail::Cleanup_S<Used_Hooks,Needed_Templates...> >...
    {
	static_assert(all<is_none<Used_Hooks,'c','v','l','r'>...>,
		      "Hook types should not have cvref qualifiers");
	
	using Deduced_Hooks =
	    Used<Type<
		detail::Match_Or_Default_Hook_S<
		    Needed_Templates,
		    Hook<Needed_Templates,Needed_Templates<Base_Types...> >,
		    Used<Used_Hooks...> > >...>;
	
	template<class... Args>
	    Validate(Args&&... hooks) :
	    Used_Hooks(fwd<Args>(hooks))...
	{}
    };
}
