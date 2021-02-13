#pragma once

/*

Goal: for any expressions x, y, z, etc, have the following just work:

for (auto&& [x_i,y_i,z_i] : zip(x,y,z))
    use(x_i,y_i,z_i);

This should iterate until x is completed, and assumes that the iterators for the other expressions are valid incrementing that many times.

We must be careful to not discard x, y, and z if they are temporaries. To do this, we use C++ aggregates, which can bind members to temporaries.

Another function-like macro zip_earliest could stop as soon as any iterator is finished, and zip_assert_same could assert if the iterators don't all have the same length.

*/

#include "Construct.hpp"
#include "Indexing.hpp"
#include "Tuple.hpp"
#include "TypeUtils.hpp"

#include <cstddef> /* std::size_t */


namespace detail {
    template<std::size_t i, class T>
    struct ZipLeaf {
	T&& t;
	T&& get() {
	    return fwd<T>(t);
	}
    };

    template<class I, class... Ts>
    struct ZipIterator;
    template<std::size_t... i, class... Ts>
    struct ZipIterator<Indices<i...>,Ts...>
	: SmartTuple<decltype(declval<ZipLeaf<i,List_Element<i,Ts...> > >().get().begin())...> {
	using Base = SmartTuple<decltype(declval<ZipLeaf<i,List_Element<i,Ts...> > >().get().begin())...>;
	template<class... Args>
	ZipIterator(Args&&... args)
	    : Base(fwd<Args>(args)...) {
	}
	DEFAULT_COPY(ZipIterator);
	template<class End_0>
	bool operator!=(End_0&& end_0) {
	    return Base::template get<0>() != fwd<End_0>(end_0);
	}
	void operator++() {
	    (++Base::template get<i>(),...);
	}
	ZipIterator& operator*() {
	    return *this;
	}
	template<SZ j> decltype(auto) get() {
	    return *Base::template get<j>();
	}
	template<SZ j> decltype(auto) get() const {
	    // While const iterator normally doesn't mean const data,
	    // here we protect against a const structured binding
	    // returning a regular reference (which it can)
	    // by returning the data as const.
	    // https://stackoverflow.com/questions/49308674/variables-marked-as-const-using-structured-bindings-are-not-const
	    return std::as_const(*Base::template get<j>());
	}
    };
    
    template<class I, class... Ts>
    struct ZipBase;
    
    template<std::size_t... i, class... Ts>
    struct ZipBase<Indices<i...>,Ts...> : public ZipLeaf<i,Ts>... {
	template<SZ j> using Element = List_Element<j,Ts...>;
    private:
	template<SZ j> using Leaf    = ZipLeaf<j,Element<j> >;
    public:
	ZipIterator<Indices<i...>,Ts...> begin() {
	    return {Leaf<i>::get().begin()...};
	}
	decltype(auto) end() {
	    static_assert(sizeof...(i) > 0,"Cannot zip nothing");
	    return Leaf<0>::get().end();
	}
    };
} /* namespace detail */

/* detail::Zip */
namespace detail {
    template<class... Ts> struct Zip
	: detail::ZipBase<Make_Indices<sizeof...(Ts)>,Ts...> {
#ifdef __clang__
	static_assert(none<is<Ts&&,'r'>...>,"zip may not bind to temporaries with clang. See https://bugs.llvm.org/show_bug.cgi?id=42220");
#endif
    };
    template<class... Ts> Zip(Ts&&...) -> Zip<Ts...>;
} /* namespace detail */

/* detail::ZipNoTmpRvalues */
namespace detail {
    template<class... Ts> struct ZipNoTmpRvalues
	: detail::ZipBase<Make_Indices<sizeof...(Ts)>,Ts...> {
    };
    template<class... Ts> ZipNoTmpRvalues(Ts&&...) -> ZipNoTmpRvalues<Ts...>;
} /* namespace detail */

namespace std {
    template<class I, class... Ts>
    class tuple_size<detail::ZipIterator<I,Ts...> > {
      public:
	static constexpr SZ value = sizeof...(Ts);
    };
    
    template<SZ i, class I, class... Ts>
    class tuple_element<i,detail::ZipIterator<I,Ts...> > {
      public:
	using type = decltype(declval<detail::ZipIterator<I,Ts...> >().template get<i>());
    };
    
    template<SZ i, class I, class... Ts>
    class tuple_element<i,detail::ZipIterator<I,Ts...> const> {
      public:
	using type = decltype(declval<detail::ZipIterator<I,Ts...> const>().template get<i>());
    };
} /* namespace std */



// lowercase so it's intuitive to use as a function
#define zip(...)						\
    _Pragma("clang diagnostic push")				\
    _Pragma("clang diagnostic ignored \"-Wmissing-braces\"")	\
    detail::Zip{__VA_ARGS__}					\
    _Pragma("clang diagnostic pop")

#define zipNoTmpRvalues(...)					\
    _Pragma("clang diagnostic push")				\
    _Pragma("clang diagnostic ignored \"-Wmissing-braces\"")	\
    detail::ZipNoTmpRvalues{__VA_ARGS__}			\
    _Pragma("clang diagnostic pop")

