/*

Goal: for any expressions x, y, z, etc, have the following just work:

for (auto&& [x_i,y_i,z_i] : Zip{x,y,z})
    use(x_i,y_i,z_i);

This should iterate until x is completed, and assumes that the iterators for the other expressions are validincrementing that many times.

We must be careful to not discard x, y, and z if they are temporaries. To do this, we use C++ aggregates, which can bind members to temporaries.

Another class Zip_Earliest could stop as soon as any iterator is finished, and Zip_Assert could assert if the iterators don't all have the same length.

*/

#include "TypeUtils.hpp"

#include <cstddef> /* std::size_t */
#include <tuple>

/* ZipIterator and ZipEntry */
template<class A, class... More> struct ZipIterator;
template<class A, class... More> struct ZipEntry {
    ZipIterator<A,More...>& iterator;
    // need std::tuple_size<Entry> and Entry.get<i>() and std::tuple_element<i,Entry>::type
    template<std::size_t i> decltype(auto) get() {
	return iterator.template getValue<i>();
    }
};
namespace std {
template<class... Args> class tuple_size<ZipEntry<Args...> > {
  public:
    static constexpr std::size_t value = sizeof...(Args);
};
template<std::size_t i, class... Args> class tuple_element<i,ZipEntry<Args...> > {
  public:
    using type = decltype(declval<ZipEntry<Args...> >().template get<i>());
};
}
template<class A> struct ZipIterator<A> {
    using A_Iterator = decltype(declval<A&&>().begin());
    using Entry = ZipEntry<A>;
    A_Iterator a_iterator;
    ZipIterator& operator++() {
	++a_iterator;
	return *this;
    }
    template<std::size_t> decltype(auto) getValue() {
	return *a_iterator;
    }
    Entry operator*() {
	return {*this};
    }
    template<class End>
    bool operator!=(End&& end) {
	return a_iterator != end;
    }
};
template<std::size_t i, class A, class... More> struct ZipIteratorGetValue {
    using A_Iterator = decltype(declval<A&&>().begin());
    using RestIterator = ZipIterator<More...>;
    static decltype(auto) getValue(A_Iterator&, RestIterator& rest_iterator) {
	return rest_iterator.template getValue<i-1>();
    }
};
template<class A, class... More> struct ZipIteratorGetValue<0,A,More...> {
    using A_Iterator = decltype(declval<A&&>().begin());
    using RestIterator = ZipIterator<More...>;
    static decltype(auto) getValue(A_Iterator& a_iterator ,RestIterator&) {
	return *a_iterator;
    }
};
template<class A, class... More> struct ZipIterator {
    using A_Iterator = decltype(declval<A&&>().begin());
    using RestIterator = ZipIterator<More...>;
    using Entry = ZipEntry<A,More...>;
    
    A_Iterator a_iterator;
    RestIterator rest_iterator;
    
    ZipIterator& operator++() {
	++a_iterator;
	++rest_iterator;
	return *this;
    }
    template<std::size_t i> decltype(auto) getValue() {
	return ZipIteratorGetValue<i,A,More...>::getValue(a_iterator,rest_iterator);
    }
    Entry operator*() {
	return {*this};
    }
    template<class End>
    bool operator!=(End&& end) {
	return a_iterator != end;
    }
};
/*
template<class A, class... More> template<> decltype(auto) ZipIterator<A,More...>::get<0>() {
    return *a_iterator;
}
*/

/* Zip */
template<class A, class... More> struct Zip;
template<class A> struct Zip<A> {
    A&& a;

    ZipIterator<A> begin() {
	static_assert(std::is_aggregate_v<Zip>,"Zip must be an aggregate");
	return {static_cast<A&&>(a).begin()};
    }

    decltype(auto) end() {
	return static_cast<A&&>(a).end();
    }
};
template<class A, class... More> struct Zip {
    using Rest = Zip<More...>;
    
    A&& a;
    Rest rest;

    ZipIterator<A,More...> begin() {
	static_assert(std::is_aggregate_v<Zip>,"Zip must be an aggregate");
	return {static_cast<A&&>(a).begin(),rest.begin()};
    }

    decltype(auto) end() {
	return static_cast<A&&>(a).end();
    }
};
template<class... Args> Zip(Args&&...) -> Zip<Args...>;

