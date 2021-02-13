#pragma once

// Construct::piecewise
// Construct::tuplewise
// TUPLE_CONSTUCTOR_IMPL(Class)
// TUPLE_CONSTUCTOR_DECL(Class)
// DEFAULT_COPY(Class)
// DELETE_COPY(Class)
// DEFAULT_ASSIGN(Class)
// DELETE_ASSIGN(Class)
// DEDUCE_COPY(Class)

namespace Construct {
    // Construction types. Use the constexpr variables below.
    struct Piecewise {};
    struct Tuplewise {};
    struct Defaultwise {};

    // Pass this as the first argument to a SmartTuple constructor for special construction.
    // In particular, when constructing SmartTuple<X,Y,Z>:
    // SmartTuple<X,Y,Z> t1{Construct::piecewise,
    //                      RefTuple{x1,x2},
    //                      RefTuple{y1},
    //                      RefTuple{z1,z2,z3}};
    constexpr Piecewise piecewise;
    
    // Pass this as the first argument to a T constructor for special construction.
    // My_abc t2{Construct::tuplewise,
    //           RefTuple{a,b,c}};
    constexpr Tuplewise tuplewise;

    // Pass this as the only argument for special construction,
    // which delegates to constructors with 0 arguments.
    constexpr Defaultwise defaultwise;

    namespace detail
    {
	struct Piecewise {};
	constexpr Piecewise piecewise;
    }
} /* namespace Construct */

// Use these macros in your class to declare constructors that can
// take Constructor::tuplewise and a tuple of arguments {args...}
// which this forwards to a regular constructor(args...)
// Typically the IMPL would be in a private section.
#define TUPLE_CONSTRUCTOR_IMPL(Class)					\
    template<SZ... _tuple_i, class _Tuple>				\
    Class(Construct::Tuplewise, ::Indices<_tuple_i...>, _Tuple&& _tuple) \
	: Class{fwd<_Tuple>(_tuple).template get<_tuple_i>()...}

#define TUPLE_CONSTRUCTOR_DECL(Class)			\
    template<class _Tuple>				\
    Class(Construct::Tuplewise, _Tuple&& _tuple)	\
	: Class{Construct::tuplewise,			\
	    typename No_cvref<_Tuple>::Indices{},	\
	    fwd<_Tuple>(_tuple)}

// Defines copy constructors
#define DEFINE_COPY(Class,defined)		\
    Class(Class       & ) = defined;		\
    Class(Class const & ) = defined;		\
    Class(Class       &&) = defined

// Defines default copy constructors
#define DEFAULT_COPY(Class) DEFINE_COPY(Class,default)

// Deleted default copy constructors
#define DELETE_COPY(Class) DEFINE_COPY(Class,delete)

// Defines default assignment operators for all c/ref combinarions
#define DEFINE_ASSIGN(Class,defined)		\
    Class& operator=(Class       & ) = defined;	\
    Class& operator=(Class const & ) = defined;	\
    Class& operator=(Class       &&) = defined

// Defines default assignment operators for all c/ref combinarions
#define DEFAULT_ASSIGN(Class) DEFINE_ASSIGN(Class,default)

// Deleted default assignment operators for all c/ref combinarions
#define DELETE_ASSIGN(Class) DEFINE_ASSIGN(Class,delete)

// Defines copy deduction guides for a template taking Class<T...> for classes T...
#define DEDUCE_COPY(Class)						\
    template<class... T> Class(Class<T...>       & ) -> Class<T...>;	\
    template<class... T> Class(Class<T...> const & ) -> Class<T...>;	\
    template<class... T> Class(Class<T...>       &&) -> Class<T...>;	\
    template<class... T> Class(Class<T...> const &&) -> Class<T...>
    
