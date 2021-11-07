#pragma once

#include "../AssertUtils.hpp"
#include "../TypeUtils.hpp"

namespace detail {

    template<SZ i> struct Index { static constexpr SZ value = i; };
    
    template<SZ... i> using Indices = std::index_sequence<i...>;
    
    template<SZ i> using Make_Indices = std::make_index_sequence<i>;

#ifdef __clang__
    // O(n) per instantiation, so accessing all elements is O(n^2)
    template<SZ i, class... T> using List_Element = __type_pack_element<i,T...>;
#else
    // O(n) per instantiation, and O(n) instantiations per index, so accessing all elements is O(n^3)
    template<SZ i, class... T> struct List_Element_S;
    template<class X, class... T> struct List_Element_S<0, X, T...> { using Type = X; };
    template<SZ i, class X, class... T> struct List_Element_S<i, X, T...> : List_Element_S<i-1, T...> {};
    template<SZ i, class... T> using List_Element = typename List_Element_S<i, T...>::Type;
#endif

    template<SZ x, class I> constexpr SZ get_index = static_cast<SZ>(-1);
    template<SZ x, SZ... i> static constexpr SZ get_index<x,Indices<i...> > =
	List_Element<x,Index<i>...>::value;

    template<class I, class J> struct Get_Indices_S;
    template<SZ... i, class J> struct Get_Indices_S<Indices<i...>,J>
    { using Type = Indices<get_index<i,J>...>; };
    template<class I, class J> using Get_Indices = Type<Get_Indices_S<I,J> >;

    template<class I> struct Combined_Sum;
    template<SZ... i> struct Combined_Sum<Indices<i...> >
    { static constexpr SZ value = (0 + ... + i); };
    template<class I> constexpr SZ combined_sum = Combined_Sum<I>::value;

    template<class I> struct All_Same_Index;
    template<SZ x, SZ... i> struct All_Same_Index<Indices<x,i...> >
    {
	static_assert(all<x==i...>,"All indices should match");
	static constexpr SZ value = x;
    };
    template<class I> constexpr SZ all_same_index = All_Same_Index<I>::value;
    
    template<class I, class... X> struct Sum_Indices_S;
    template<SZ... i, class... X> struct Sum_Indices_S<Indices<i...>,X...>
    {
	template<SZ j> static constexpr SZ value = (0 + ... + get_index<j,X>);
	using Type = Indices<value<i>...>;
    };
    template<class... X> using Sum_Indices =
	Type<Sum_Indices_S<Make_Indices<all_same_index<Indices<X::size()...> > >,X...> >;
    
    template<class I, class X, class Y> struct Subtract_Indices_S;
    template<SZ... i, class X, class Y> struct Subtract_Indices_S<Indices<i...>,X,Y>
    { using Type = Indices<get_index<i,X> - get_index<i,Y>...>; };
    template<class X, class Y> using Subtract_Indices =
	Type<Subtract_Indices_S<Make_Indices<all_same_index<Indices<X::size(),Y::size()> > >,X,Y> >;

    template<SZ i, class I> constexpr SZ sum_of_first =
	sum_of_first<i-1,I> + get_index<i-1,I>;
    template<class I> static constexpr SZ sum_of_first<0,I> = 0;
    
    template<class I, class X> struct Prefix_Sum_Inclusive_S;
    template<SZ... i, class X> struct Prefix_Sum_Inclusive_S<Indices<i...>,X>
    { using Type = Indices<sum_of_first<i+1,X>...>; };
    template<class I> using Prefix_Sum_Inclusive =
	Type<Prefix_Sum_Inclusive_S<Make_Indices<I::size()>,I> >;
    
    template<class I, class X> struct Prefix_Sum_Exclusive_S;
    template<SZ... i, class X> struct Prefix_Sum_Exclusive_S<Indices<i...>,X>
    { using Type = Indices<sum_of_first<i,X>...>; };
    template<class I> using Prefix_Sum_Exclusive =
	Type<Prefix_Sum_Exclusive_S<Make_Indices<I::size()>,I> >;
    
    template<SZ x, class I> struct Copy_Index_Onto;
    template<SZ x, SZ... i> struct Copy_Index_Onto<x,Indices<i...> >
    { using Type = Indices< i>=0?x:x...>; };
    template<SZ x, SZ n> using Repeat_Index =
	Type<Copy_Index_Onto<x,Make_Indices<n> > >;

    template<class I, class M> struct Sum_GEQ_S;
    template<SZ... i, SZ... m> struct Sum_GEQ_S<Indices<i...>,Indices<m...> >
    {
	template<SZ j> static constexpr SZ value = (0 + ... + (j>=m));
	using Type = Indices<value<i>...>;
    };
    template<SZ len, class I> using Sum_GEQ = Type<Sum_GEQ_S<Make_Indices<len>,I> >;
    
    template<class I> using Outer_Flatten_Indices =
	Sum_GEQ<combined_sum<I>,Prefix_Sum_Inclusive<I> >;
    
    template<class I> using Inner_Flatten_Indices =
	Subtract_Indices<Make_Indices<combined_sum<I> >,
			 Get_Indices<Outer_Flatten_Indices<I>,Prefix_Sum_Exclusive<I> > >;
    
    template<class Outer, class Inner, class... I> struct Concat_Indices_S;
    template<SZ... o, SZ... i, class... I>
    struct Concat_Indices_S<Indices<o...>,Indices<i...>,I...>
    { using Type = Indices<get_index<i,List_Element<o,I...> >...>; };
    template<class... I> using Concat_Indices =
	Type<Concat_Indices_S<Outer_Flatten_Indices<Indices<I::size()...> >,
			      Inner_Flatten_Indices<Indices<I::size()...> >,
			      I...> >;

} /* namespace detail */
