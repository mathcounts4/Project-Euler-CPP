#pragma once

#include "TypeUtils.hpp"

/*
Meant for use in range-based for loops.
If aggregate initialization {} is used, this will extend the lifetime of a temporary argument to the end of the loop.

Example:
std::map<int,int> m{{1,2},{5,8},{10,11}};
for (int sum : ContainerTransformer{m,[](auto&& pair){return pair.first + pair.second; }})
...
*/
template<class Container, class Functor> struct ContainerTransformer
{
    // aggregate initialization used to extend lifetimes of container and functor, if temporary
    Container&& container;
    Functor&& functor;

    using Base_Iterator = No_cvref<decltype(declval<Container&&>().begin())>;

    struct iterator
    {
	Functor&& functor;
	Base_Iterator it;
	bool operator!=(iterator const & other) const {
	    return it != other.it;
	}

	iterator& operator++() {
	    ++it;
	    return *this;
	}

	decltype(auto) operator*() const {
	    return fwd<Functor>(functor)(*it);
	}
    };

    iterator begin() const {
	return {fwd<Functor>(functor),fwd<Container>(container).begin()};
    }

    iterator end() const {
	return {fwd<Functor>(functor),fwd<Container>(container).end()};
    }

    using const_iterator = iterator;
};
template<class Container, class Functor> ContainerTransformer(Container&&,Functor&&) -> ContainerTransformer<Container,Functor>;
