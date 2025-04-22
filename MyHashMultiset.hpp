#pragma once

#include <set>

#include "MyHashUtil.hpp"

template<class... Args>
struct std::hash<std::multiset<Args...> > {
    std::size_t operator()(std::multiset<Args...> const& x) const {
	std::size_t ans = 0;
	for (auto const& t : x)
	    my_hash_combine(ans, t);
	return ans;
    }
};
