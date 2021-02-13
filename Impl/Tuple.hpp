#pragma once

#include "../Class.hpp"
#include "../Construct.hpp"
#include "../Indexing.hpp"
#include "../StringUtils.hpp"
#include "../TypeUtils.hpp"

template<class... T> struct BasicTuple;
template<class... T> struct RefTuple;
template<class... T> struct SmartTuple;

namespace tuple {

/* TupleElement<Tuple,i> */
template<class Tuple, SZ i> using TupleElement = typename No_cvref<Tuple>::template Element<i>;
    
namespace detail {
    
template<class I, class T, class V = void> struct Expand_Indices_Get;
template<SZ... i, class T> struct Expand_Indices_Get<
    Indices<i...>,T,std::void_t<decltype(declval<T&>().template get<i>())...> > {
    using Type = void;
};
template<class T, class V = void> struct Acts_As_Tuple {
    static constexpr B value = false;
};
template<class T> struct Acts_As_Tuple<T,std::void_t<Type<Expand_Indices_Get<Make_Indices<T::size>,T> > > > {
    static constexpr B value = true;
};
} /* namespace detail */

template<class T> constexpr B acts_as_Tuple = detail::Acts_As_Tuple<T>::value;
template<class T> constexpr B acts_as_cvref_Tuple = acts_as_Tuple<No_cvref<T> >;
template<class A> using Tuple_Enclosed =
    Cond<acts_as_cvref_Tuple<A>,A&&,RefTuple<A&&> >;
} /* namespace tuple */
template<class A> tuple::Tuple_Enclosed<A> tuple_enclose(A&& a) {
    return fwd<A>(a);
}

namespace tuple {
/* TupleLeaf */
template<SZ i, class T> struct TupleLeaf {
  private:
    T t;

  protected:
    template<class... A> TupleLeaf(A&&... a) : t{fwd<A>(a)...} {}

    DEFAULT_COPY(TupleLeaf);
    DEFAULT_ASSIGN(TupleLeaf);

    T       &  get()       &  { return t; }
    T const &  get() const &  { return t; }
    T       && get()       && { return fwd<T>(t); }
    T const && get() const && { return fwd<T const>(t); }
};
/* SmartTupleLeaf */
template<SZ i, class T> struct SmartTupleLeaf : protected TupleLeaf<i,T> {
  private:
    TUPLE_CONSTRUCTOR_IMPL(SmartTupleLeaf) {}
  protected:
    /* Same as TupleLeaf */
    template<class... A> SmartTupleLeaf(A&&... a) : TupleLeaf<i,T>{fwd<A>(a)...} {}
    /* But can also accept Construct::tuplewise and a tuple to expand */
    TUPLE_CONSTRUCTOR_DECL(SmartTupleLeaf) {}
    /* And can also accept Construct::defaultwise */
    SmartTupleLeaf(Construct::Defaultwise) : TupleLeaf<i,T>{} {}

    DEFAULT_COPY(SmartTupleLeaf);
    DEFAULT_ASSIGN(SmartTupleLeaf);
};

/* TupleImpl */
template<template<SZ,class> class LeafImpl, class I, class... T>
struct TupleImpl;
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
struct TupleImpl<LeafImpl,Indices<i...>,T...> : private LeafImpl<i,T>... {
public:
    static constexpr SZ size     = sizeof...(T);
    static_assert(size == sizeof...(i),
		  "Tuple indices and Types must have same lengths");
    using Indices                = ::Indices<i...>;
    
    template<SZ j> using Element = List_Element<j,T...>;
private:
    template<SZ j> using Leaf    = LeafImpl<j,Element<j> >;

protected:
    /* size arguments - forward each to the corresponding element */
    template<class... A> TupleImpl(A&&... a) : Leaf<i>{fwd<A>(a)}... {}

    /* Special piecewise tuple constructor */
    template<class... Ts> TupleImpl(Construct::detail::Piecewise,Ts&&... ts)
	: Leaf<i>{Construct::tuplewise,fwd<Ts>(ts)}... {}

    /* No arguments - default construct everything */
    TupleImpl() : Leaf<i>{}... {}

    DEFAULT_COPY(TupleImpl);
    DEFAULT_ASSIGN(TupleImpl);

public:
    /* get<j>() */
    template<SZ j> Element<j>       &  get()       &  { return               Leaf<j>::get(); }
    template<SZ j> Element<j> const &  get() const &  { return               Leaf<j>::get(); }
    template<SZ j> Element<j>       && get()       && { return ::move(*this).Leaf<j>::get(); }
    template<SZ j> Element<j> const && get() const && { return ::move(*this).Leaf<j>::get(); }

    /* ref() */
    auto ref()       & ;
    auto ref() const & ;
    auto ref()       &&;
    auto ref() const &&;

    /* ref_select<true,false,...> retuns a tuple with references to the true elements */
    template<B... b> auto ref_select()       & ;
    template<B... b> auto ref_select() const & ;
    template<B... b> auto ref_select()       &&;
    template<B... b> auto ref_select() const &&;
};
} /* namespace tuple */

template<template<SZ,class> class LeafImpl, SZ... i, class... T>
struct Class<tuple::TupleImpl<LeafImpl,Indices<i...>,T...> > {
    using Type = tuple::TupleImpl<LeafImpl,Indices<i...>,T...>;
    // all formatting for TupleImpl is done without enclosing {}
    static std::ostream& print(std::ostream& oss, Type const & t) {
	return oss << string_concat(",",{(::to_string(t.template get<i>()) + ::cvref_string<T>())...}).c_str();
    }
    static std::string name() {
	return string_concat(",",{(Class<No_cvref<T> >::name() + ::cvref_string<T>())...});
    }
    static std::string format() {
	static_assert(all<!is_any<T,'l','r'>...>,"To use input format for a tuple, no types can be references");
	return string_concat(",",{Class<No_cvref<T> >::format()...});
    }
};

/* BasicTuple definition */
template<class... T> struct BasicTuple
    : public tuple::TupleImpl<tuple::TupleLeaf,Make_Indices<sizeof...(T)>,T...> {
    using Base               = tuple::TupleImpl<tuple::TupleLeaf,Make_Indices<sizeof...(T)>,T...>;
    static constexpr SZ size = Base::size;
    BasicTuple(T const &... t) : Base{t...} {}
    template<class... A> BasicTuple(A&&... a) : Base{fwd<A>(a)...} {}

    DEFAULT_COPY(BasicTuple);
    DEFAULT_ASSIGN(BasicTuple);
};
template<class... T> BasicTuple(T&&...) -> BasicTuple<No_cvref<T>...>;
DEDUCE_COPY(BasicTuple);

template<class... T> struct Class<BasicTuple<T...> > {
    using Type = BasicTuple<T...>;
    using Base = typename Type::Base;
    static std::ostream& print(std::ostream& oss, Type const & t) {
	oss << '{';
        Class<Base>::print(oss,t);
	oss << '}';
	return oss;
    }
    static Optional<Type> parse(std::istream& is) {
	static_assert(all<!is_any<T,'l','r'>...>,"To parse a tuple, no types can be references");
	Resetter resetter{is};
	try {
	    consume_opt(is,'{').deref_without_stack();
	    bool started = false;
	    Type value{
		(started ?
		 (void) consume_opt(is,',').deref_without_stack(), false :
		 started = true,
		 Class<T>::parse(is)).deref_without_stack()...};
	    consume_opt(is,'}').deref_without_stack();
	    resetter.ignore();
	    return std::move(value);
	} catch (std::exception const & failure) {
	    return Failure(name() + " parse error:\n" + failure.what());
	} catch (...) {
	    return Failure(name() + " parse error: unknown exception");
	}
    }
    static std::string name() {
	return "BasicTuple<" + class_name<Base>() + ">";
    }
    static std::string format() {
	return '{' + Class<Base>::format() + '}';
    }
};

/* RefTuple definition */
template<class... T> struct RefTuple : BasicTuple<T...> {
  private:
    static_assert(all<is_any<T,'l','r'>...>,
		  "All types in RefTuple should be references");
    using Base = BasicTuple<T&&...>;
  public:
    static constexpr SZ size = Base::size;
    RefTuple(T&&... t) : Base{fwd<T>(t)...} {}
    
    DEFAULT_COPY(RefTuple);
    DEFAULT_ASSIGN(RefTuple);
};
template<class... T> RefTuple(T&&...) -> RefTuple<T&&...>;
DEDUCE_COPY(RefTuple);

namespace tuple {
/* TupleImpl's ref() function definitions */
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref() & {
    return RefTuple<Element<i>&...>{get<i>()...};
}
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref() const & {
    return RefTuple<Element<i> const & ...>{get<i>()...};
}
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref() && {
    return RefTuple<Element<i>&&...>{::move(*this).template get<i>()...};
}
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref() const && {
    return RefTuple<Element<i> const &&...>{::move(*this).template get<i>()...};
}
} /* namespace tuple */

/* SmartTuple definition */
template<class... T> struct SmartTuple
    : public tuple::TupleImpl<tuple::SmartTupleLeaf,Make_Indices<sizeof...(T)>,T...> {
  private:
    TUPLE_CONSTRUCTOR_IMPL(SmartTuple) {}
    using Base = tuple::TupleImpl<tuple::SmartTupleLeaf,Make_Indices<sizeof...(T)>,T...>;
    
  public:
    static constexpr SZ size = Base::size;
    
    SmartTuple(Construct::Defaultwise) : Base{} {}
    template<class... A> SmartTuple(A&&... a) : Base{fwd<A>(a)...} {}
    template<class... Ts> SmartTuple(Construct::Piecewise, Ts&&... ts)
	: Base{Construct::detail::piecewise,tuple_enclose(fwd<Ts>(ts))...} {}
    TUPLE_CONSTRUCTOR_DECL(SmartTuple) {}
    
    DEFAULT_COPY(SmartTuple);
    DEFAULT_ASSIGN(SmartTuple);
};
template<class... T> SmartTuple(T&&...) -> SmartTuple<No_cvref<T>...>;
DEDUCE_COPY(SmartTuple);



template<B move, class T>
constexpr decltype(auto) move_if(T&& t) {
    return static_cast<Cond<move,No_ref<T>&&,T&&> >(t);
}

namespace detail {
template<template<class...> class TupleType, B move_args, SZ... i, class SuperTuple, SZ... j>
auto tuple_cat_indices(Indices<i...>, Indices<j...>, SuperTuple&& st) {
    using Result = TupleType<tuple::TupleElement<tuple::TupleElement<SuperTuple,i>,j>...>;
    return Result{move_if<move_args>(fwd<SuperTuple>(st).template get<i>()).template get<j>()...};
}
} /* namespace detail */

template<class... Tuples> auto tuple_cat(Tuples&&... tuples) {
    using Tuple_Sizes = Indices<No_cvref<Tuples>::size...>;
    using Outer = Outer_Flatten_Indices<Tuple_Sizes>;
    using Inner = Inner_Flatten_Indices<Tuple_Sizes>;
    return detail::tuple_cat_indices<BasicTuple,false>(
	Outer{},Inner{},RefTuple<Tuples&&...>{fwd<Tuples>(tuples)...});
}

template<class... Tuples> auto tuple_cat_move(Tuples&&... tuples) {
    using Tuple_Sizes = Indices<No_cvref<Tuples>::size...>;
    using Outer = Outer_Flatten_Indices<Tuple_Sizes>;
    using Inner = Inner_Flatten_Indices<Tuple_Sizes>;
    return detail::tuple_cat_indices<BasicTuple,true>(
	Outer{},Inner{},RefTuple<Tuples&&...>{fwd<Tuples>(tuples)...});
}

template<class... Tuples> auto tuple_cat_refs(Tuples&&... tuples) {
    return tuple_cat(fwd<Tuples>(tuples).ref()...).ref();
}



namespace detail {
template<class Functor, class Tuple, SZ... i>
void tuple_apply_all(Functor&& functor, Tuple&& tuple, Indices<i...>) {
    (fwd<Functor>(functor)(fwd<Tuple>(tuple).template get<i>()),...);
}
} /* namespace detail */
template<class Functor, class Tuple, class = std::enable_if_t<tuple::acts_as_cvref_Tuple<Tuple> > >
void tuple_apply_all(Functor&& functor, Tuple&& tuple) {
    using Indices = typename No_cvref<Tuple>::Indices;
    detail::tuple_apply_all(fwd<Functor>(functor),fwd<Tuple>(tuple),Indices{});
}



namespace detail {
template<class Functor, class Tuple, std::size_t... i>
decltype(auto) apply_helper(Functor&& functor, Tuple&& tuple, Indices<i...>) {
    return fwd<Functor>(functor)(fwd<Tuple>(tuple).template get<i>()...);
}
} /* namespace detail */
template<class Functor, class Tuple>
decltype(auto) apply_tuple(Functor&& functor, Tuple&& tuple) {
    using Indices = typename No_cvref<Tuple>::Indices;
    return detail::apply_helper(fwd<Functor>(functor),fwd<Tuple>(tuple),Indices{});
}



namespace tuple {
template<B b> struct Ref_Tuple_Or_Empty;

template<> struct Ref_Tuple_Or_Empty<true> {
    template<class T> RefTuple<T&&> operator()(T&& t) const {
	return fwd<T>(t);
    }
};

template<> struct Ref_Tuple_Or_Empty<false> {
    template<class T> RefTuple<> operator()(T&&) const {
	return {};
    }
};
    
template<B b, class T> auto ref_tuple_or_empty(T&& t) {
    return Ref_Tuple_Or_Empty<b>{}(fwd<T>(t));
}
    
/* TupleImpl's ref_select() function definitions */
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
template<B... b>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref_select() & {
    return tuple_cat_refs(ref_tuple_or_empty<b>(get<i>())...);
}
    
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
template<B... b>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref_select() const & {
    return tuple_cat_refs(ref_tuple_or_empty<b>(get<i>())...);
}
    
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
template<B... b>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref_select()       && {
    return tuple_cat_refs(ref_tuple_or_empty<b>(::move(*this).template get<i>())...);
}
    
template<template<SZ,class> class LeafImpl, SZ... i, class... T>
template<B... b>
auto TupleImpl<LeafImpl,Indices<i...>,T...>::ref_select() const && {
    return tuple_cat_refs(ref_tuple_or_empty<b>(::move(*this).template get<i>())...);
}
} /* namespace tuple */
