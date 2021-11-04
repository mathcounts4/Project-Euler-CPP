#pragma once

#include "Impl/Indexing.hpp"

/* Index<i> */
template<SZ i> using Index = detail::Index<i>;

/* Indices<i...> */
template<SZ... i> using Indices = detail::Indices<i...>;

/* Make_Indices<x> = Indices<0,1,...,x-1> */
template<SZ i> using Make_Indices = detail::Make_Indices<i>;

/* List_Element<1,X,Y,Z> = Y */
template<SZ i, class... T> using List_Element = detail::List_Element<i,T...>;

/* get_index<x,Indices<a,b,c,...> > = x'th index of a,b,c,... */
template<SZ x, class I> constexpr SZ get_index = detail::get_index<x,I>;

/* Get_Indices<Indices<2,1,1>,Indices<5,6,7,8> > = Indices<7,6,6> */
template<class I, class J> using Get_Indices = detail::Get_Indices<I,J>;

/* combined_sum<Indices<a,b,c...> > = a+b+c+... */
template<class I> constexpr SZ combined_sum = detail::combined_sum<I>;

/* all_same_index<Indices<a,b,c,...> > = a, and errors if any values are different */
template<class I> constexpr SZ all_same_index = detail::all_same_index<I>;

/* Sum_Indices<Indices<a,b,c>,Indices<d,e,f>,Indices<g,h,i> > = Indices<a+d+g,b+e+h,c+f+i> */
template<class... X> using Sum_Indices = detail::Sum_Indices<X...>;

/* Subtract_Indices<Indices<a,b,c>,Indices<d,e,f> > = Indices<a-d,b-e,c-f> */
template<class X, class Y> using Subtract_Indices = detail::Subtract_Indices<X,Y>;

/* Prefix_Sum_Inclusive<Indices<3,1,2> > = Indices<3,4,6> */
template<class I> using Prefix_Sum_Inclusive = detail::Prefix_Sum_Inclusive<I>;

/* Prefix_Sum_Exclusive<Indices<3,1,2> > = Indices<0,3,4> */
template<class I> using Prefix_Sum_Exclusive = detail::Prefix_Sum_Exclusive<I>;

/* Repeat_Index<x,3> = Indices<x,x,x> */
template<SZ x, SZ n> using Repeat_Index = detail::Repeat_Index<x,n>;

/* Outer_Flatten_Indices<Indices<3,1,2> > = Indices<0,0,0,1,2,2> */
template<class I> using Outer_Flatten_Indices = detail::Outer_Flatten_Indices<I>;

/* Inner_Flatten_Indices<Indices<3,1,2> > = Indices<0,1,2,0,0,1> */
template<class I> using Inner_Flatten_Indices = detail::Inner_Flatten_Indices<I>;

/* Concat_Indices<Indices<a,b,c>,Indices<d,e,f> > = Indices<a,b,c,d,e,f> */
template<class... I> using Concat_Indices = detail::Concat_Indices<I...>;
