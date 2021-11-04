#pragma once

#include <type_traits>

#include "AssertUtils.hpp"
#include "Construct.hpp"
#include "Tuple.hpp"
#include "TypeUtils.hpp"

namespace algorithm2 {

/* Hook stores a class Type and a Functor
 * Type is used for identifying the Hook
 *
 * Functor can be a class or reference.
 * If it's a reference, it's the caller's responsibility to ensure
 * that the functor outlives the hook.
 */
template<class Type, class Functor> struct Hook {
  private:
    TUPLE_CONSTRUCTOR_IMPL(Hook) {}
	
  public:
    static_assert(is_pure<Type>,"Hook types should be pure (no const/volatile/reference");
	
    Functor functor;
	
    template<class... Args> Hook(Args&&... args) : functor(fwd<Args>(args)...) {}
    TUPLE_CONSTRUCTOR_DECL(Hook) {}

    DELETE_COPY(Hook);
    DELETE_ASSIGN(Hook);

    // Types should be pure (no cvref)
    template<class... Types> static constexpr B is_a = any<is_same<Types,Type>...>;
	
    template<class... Args>
    decltype(auto) operator()(Args&& ... args) {
	return functor(fwd<Args>(args)...);
    }
	
    template<class... Args>
    decltype(auto) operator()(Args&& ... args) const {
	// as_const because functor could be a reference
	return std::as_const(functor)(fwd<Args>(args)...);
    }
};
template<class Type> using Default_Hook = Hook<Type,Type>;

/* Caller must ensure that the functor does not go out of scope before the hook */
template<class Type, class Functor>
Hook<Type,Functor&&> ref_hook(Functor&& functor) {
    return fwd<Functor>(functor);
}

/* Constructs an owned hook from the provided arguments in Functor construction */
template<class Type, class Functor, class... Args>
Hook<Type,Functor> owned_hook(Args&&... args) {
    static_assert(is_obj<Functor>,"Owned hooks should not be references");
    return {fwd<Args>(args)...};
}
/* Constructs an owned hook from the provided functor */
template<class Type, class Functor>
Hook<Type,No_cvref<Functor> > owned_hook(Functor&& functor) {
    return fwd<Functor>(functor);
}

/* Constructs a default owned hook, using the arguments in Functor construction */
template<class Type, class... Args>
Default_Hook<Type> default_hook(Args&&... args) {
    return {fwd<Args>(args)...};
}

/* Some structs to inherit from for easy default construction */
template<auto value> struct Constant {
    template<class... Args> constexpr inline auto
    operator()(Args&&...) const {
	return value;
    }
};
struct Identity {
    template<class T>
    inline T&& operator()(T&& t) const {
	return fwd<T>(t);
    }
};
struct Ignore {
    template<class... Args>
    inline constexpr void operator()(Args&&...) const {}
};

/* Some structs to pass as arguments to distinguish functionality */
struct Get {}; constexpr Get get;
struct Set {}; constexpr Set set;
    

/* Hooks struct stores hooks or references to hooks */
template<class... Hook_Types> struct Hooks;

/* Deduction guides for Hooks */
/* Default - assume all arguments are hooks */
template<class... Hook>
Hooks(Hook&&...) -> Hooks<Hook&&...>;
/* Rules for tuple types */
template<template<class...> class Tuple, class... Hook>
Hooks(Construct::Tuplewise, Tuple<Hook...>      & ) -> Hooks<Hook      & ...>;
template<template<class...> class Tuple, class... Hook>
Hooks(Construct::Tuplewise, Tuple<Hook...> const& ) -> Hooks<Hook const& ...>;
template<template<class...> class Tuple, class... Hook>
Hooks(Construct::Tuplewise, Tuple<Hook...>      &&) -> Hooks<Hook      &&...>;
template<template<class...> class Tuple, class... Hook>
Hooks(Construct::Tuplewise, Tuple<Hook...> const&&) -> Hooks<Hook const&&...>;
/* Copy construction also deduced */
DEDUCE_COPY(Hooks);

template<class... Args>
decltype(auto) hooks(Args&&... args) { return Hooks{fwd<Args>(args)...}; }

/* const Hooks (even when holding an owned hook) does NOT propagate const to its members.
   To use a const hook, the member itself must
   have the const qualifier (possibly ref-qualified). */
template<class... Hook_Types> struct Hooks : public SmartTuple<Hook_Types...> {
  private:
    using Base = SmartTuple<Hook_Types...>;
		
    static_assert(all<is_a<Hook,No_cvref<Hook_Types> >...>,
		  "All Hooks arguments must be of (possibly cv-ref) Hook type");
	
    TUPLE_CONSTRUCTOR_IMPL(Hooks) {}

    template<SZ... i, class... T> void call(Indices<i...>, T&&... t) const {
	(const_cast<Hooks&>(*this).template get<i>()(fwd<T>(t)...),...);
    }

  public:

    static constexpr SZ size = Base::size;
	
    template<class... Args> Hooks(Args&&... args) : Base{fwd<Args>(args)...} {}

    TUPLE_CONSTRUCTOR_DECL(Hooks) {}

    DELETE_COPY(Hooks);
    DELETE_ASSIGN(Hooks);

    /* num_of<X,...>, has_none<X,...>, is_all<X,...> */
    template<class... Desired> static constexpr SZ num_of =
	sum<No_cvref<Hook_Types>::template is_a<Desired...>...>;
    template<class... Desired> static constexpr B has_none = num_of<Desired...> == 0;
    template<class... Desired> static constexpr B is_all = num_of<Desired...> == size;

    /* .matching<X,...>() returns a Hooks with references to the hooks matching X... */
    template<class... Desired> constexpr auto matching() const {
	static_assert(all<is_pure<Desired>...>,"Desired arguments should not be cv-ref qualified");
	return hooks(Construct::tuplewise,const_cast<Hooks&>(*this).template
		     ref_select<No_cvref<Hook_Types>::template is_a<Desired...>...>());
    }

    /* .not_matching<X,...>() returns a Hooks with references to the hooks NOT matching X... */
    template<class... Desired> constexpr auto not_matching() const {
	static_assert(all<is_pure<Desired>...>,"Desired arguments should not be cv-ref qualified");
	return hooks(Construct::tuplewise,const_cast<Hooks&>(*this).template
		     ref_select<!No_cvref<Hook_Types>::template is_a<Desired...>...>());
    }

    /* .add_missing_defaults<X,...>() returns a Hooks with defaults for X,... added to the end if no existing Hooks matched each */
    template<class... Desired> auto add_missing_defaults() const {
	static_assert(all<is_pure<Desired>...>,"Desired arguments should not be cv-ref qualified");
	using Result = Hooks<Hook_Types&...,Default_Hook<Cond<has_none<Desired>,Desired,Ignore> >...>;
	return Result{Construct::tuplewise,tuple_cat_refs(RefTuple{Construct::piecewise},SmartTuple<RefTuple<Hook_Types&>...>{Construct::tuplewise,const_cast<Hooks&>(*this)},BasicTuple<Cond<false,Desired,RefTuple<> > >{}...)};
    }

    /* assert_only<X,Y> checks that all hooks match X, Y, or Ignore */
    template<class... Desired> static constexpr void assert_only() {
	static_assert(all<is_pure<Desired>...>,"Desired arguments should not be cv-ref qualified");
	using Invalid = decltype(declval<Hooks>().template not_matching<Ignore,Desired...>());
	static_assert(Invalid::size == 0,"Extra hooks of unknown type specified");
    }

    /* + appends two Hooks together, using references to the original hooks */
    template<class... Other_Hook_Types> Hooks<Hook_Types const&...,Other_Hook_Types const&...>
    operator+(Hooks<Other_Hook_Types...> const& o) const {
	return {Construct::tuplewise,tuple_cat_refs(
		const_cast<Hooks&>(*this),
		const_cast<Hooks<Other_Hook_Types...>&>(o))};
    }

    /* returns the unique hook matching the Desired type in a Hooks */
    template<class Desired> decltype(auto) unique() const {
	static_assert(is_pure<Desired>,"Desired argument should not be cv-ref qualified");
	static_assert(!has_none<Desired>,"Missing a necessary hook");
	static_assert(!(num_of<Desired> > 1),"Multiple hooks provided for unique hook");

	return const_cast<Hooks&>(*this).matching<Desired>().template get<0>();
    }

    // applies each of the hooks to the input arguments
    template<class... T> void operator()(T&&... t) const {
	call(Make_Indices<sizeof...(Hook_Types)>{},fwd<T>(t)...);
    }
};

#define EACH_CVREF(MACRO) MACRO(&) MACRO(const&) MACRO(&&) MACRO(const&&)
#define PLUS_SECOND(CVREF)						\
    template<class... Args, class Type, class Functor>			\
    auto operator+(Hooks<Args...> const& hs, Hook<Type,Functor> CVREF h) { \
	return hs + Hooks{fwd<decltype(h)>(h)};				\
    }

EACH_CVREF(PLUS_SECOND);
#undef PLUS_SECOND
    
#define PLUS_FIRST(CVREF)						\
    template<class... Args, class Type, class Functor>			\
    auto operator+(Hook<Type,Functor> CVREF h, Hooks<Args...> const& hs) { \
	return Hooks{fwd<decltype(h)>(h)} + hs;				\
    }

EACH_CVREF(PLUS_FIRST);
#undef PLUS_FIRST
#undef EACH_CVREF

#define EACH_CVREF_TWO_ARGS(MACRO)					\
    MACRO(&,&) MACRO(&,const&) MACRO(&,&&) MACRO(&,const&&)		\
	MACRO(const&,&) MACRO(const&,const&) MACRO(const&,&&) MACRO(const&,const&&) \
	MACRO(&&,&) MACRO(&&,const&) MACRO(&&,&&) MACRO(&&,const&&)	\
	MACRO(const&&,&) MACRO(const&&,const&) MACRO(const&&,&&) MACRO(const&&,const&&)

#define PLUS_BOTH(CVREF1,CVREF2)					\
    template<class T1, class F1, class T2, class F2>			\
    auto operator+(Hook<T1,F1> CVREF1 h1, Hook<T2,F2> CVREF2 h2) {	\
	return Hooks{fwd<decltype(h1)>(h1),fwd<decltype(h2)>(h2)};	\
    }

    EACH_CVREF_TWO_ARGS(PLUS_BOTH);
#undef PLUS_BOTH
#undef EACH_CVREF_TWO_ARGS

} /* namespace algorithm2 */
