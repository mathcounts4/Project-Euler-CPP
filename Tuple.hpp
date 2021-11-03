#pragma once

#include "Impl/Tuple.hpp"
#include "TypeUtils.hpp"

// BasicTuple, RefTuple, and SmartTuple

// BasicTuple stores anything, and deduces pure object types.
// RefTuple stores references.
// SmartTuple stores anything, and can be piecewise-constructed
//     in order to forward arguments to construct elements in-place.

// SmartTuple<X,Y,Z> t1{Construct::piecewise,
//                      RefTuple{x1,x2},
//                      y1,
//                      RefTuple{z1,z2,z3}};

// In addition to Construct::piecewise, we also introduce Construct::tuplewise.
// With a special constructor for T taking in Construct::tuplewise and a tuple of arguments, this delegates to the constructor taking those arguments.

// SmartTuple<X,Y,Z> t2{Construct::tuplewise,
//                      RefTuple{x,y,z}};

// Use the .get<i>() method on any tuple instance to get the data at index i.
// Use the .refs() method on any tuple instance to get a tuple of references to its data.
// Use .size on any tuple (static constexpr std::size_t) to get the number of elements.


/* BasicTuple, RefTuple, SmartTuple */
template<class... T> struct BasicTuple;
template<class... T> struct RefTuple;
template<class... T> struct SmartTuple;

/* TupleElement<TupleType, index> */
template<class Tuple, SZ i> using TupleElement = tuple::TupleElement<Tuple,i>;

/* acts_as_Tuple<Type> */
template<class T> constexpr B acts_as_Tuple = tuple::acts_as_Tuple<T>;

// If a tuple type, returns the argument, otherwise encloses in a RefTuple
template<class A> tuple::Tuple_Enclosed<A> tuple_enclose(A&& a);

/* Concatenates the input tuples, yielding a BasicTuple of the results */
/* Each element is directly constructed from the corresponding element */
template<class... Tuples> auto tuple_cat(Tuples&&... tuples);

/* Concatenates the input tuples, yielding a BasicTuple of the results */
/* Each non-reference element is moved into the corresponding element */
template<class... Tuples> auto tuple_cat_move(Tuples&&... tuples);

/* Concatenates the input tuples into a RefTuple of the results */
/* The cvref qualifiers on the tuples themselves are used:
 * BasicTuple<int,char&&> const& -> RefTuple<int const&,char&> */
template<class... Tuples> auto tuple_cat_refs(Tuples&&... tuples);

/* Applies the functor (forwarded) to each element of the tuple (forwarded), in order. */
template<class Functor, class Tuple, class>
void tuple_apply_all(Functor&& functor, Tuple&& tuple);

// Calls the functor with the tuple members as arguments
template<class Functor, class Tuple>
decltype(auto) apply_tuple(Functor&& functor, Tuple&& tuple);

// Calls the functors in the first tuple with 
