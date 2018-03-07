#pragma once

#ifndef MY_HASH_VECTOR
#define MY_HASH_VECTOR

#include "MyHashUtil.hpp"

template<class T>
struct std::hash<std::vector<T> > {
    std::size_t operator()(std::vector<T> const & x) const {
	std::size_t ans = 0;
	for (T const & t : x)
	    my_hash_combine(ans,t);
	return ans;
    }
};

#endif /* MY_HASH_VECTOR */
