/* This creates a std::hash template type for your custom class

In the custom class file:

1. Include this header
2. In your class definition, place the following line:
    friend std::hash<MyClass>;
3. After your class definition, use one of the defined macros:
3a. If using a non-template custom class:
    MAKE_HASHABLE(MyClass,x,x.member1,x.member2,...);
3b. If using a template custom class:
    MAKE_HASHABLE_TEMPLATE(<class A COMMA_MACRO class B>,MyClass<A,B>,x,x.member1,...);
*/

#pragma once

#include <functional> /* std::hash */
#include <map>
#include <tuple> /* std::apply */
#include <utility> /* std::forward */

#define COMMA_MACRO ,

#define MAKE_HASHABLE_TEMPLATE(hashTypes,Class,obj,...)		\
    template hashTypes						\
    struct std::hash<Class> {					\
	std::size_t operator()(Class const& obj) const {	\
	    std::size_t ans = 0;				\
	    my_hash_combine(ans,__VA_ARGS__);			\
	    return ans;						\
	}							\
    }

#define MAKE_HASHABLE(Class,obj,...)			\
    MAKE_HASHABLE_TEMPLATE(<>,Class,obj,__VA_ARGS__)

inline void my_hash_combine(std::size_t&) {}

template<class T, class... Args>
inline void my_hash_combine(std::size_t& seed,
			    T const& t,
			    Args const&... args) {
    seed ^= std::hash<T>()(t) + 0x9e3779b97f4a7c16 + (seed<<6) + (seed>>2);
    my_hash_combine(seed, args...);
}


template<class T, std::size_t N>
struct std::hash<std::array<T, N>> {
    std::size_t operator()(std::array<T, N> const& a) const {
	std::size_t ans = 0;
	for (auto const& x : a) {
	    my_hash_combine(ans, x);
	}
	return ans;
    }
};

template<class... Args>
struct std::hash<std::tuple<Args...> > {
    std::size_t operator()(std::tuple<Args...> const& t) const {
	std::size_t ans = 0;
	auto apply_this = [&ans](Args&&... args) -> void {
	    return my_hash_combine(ans,static_cast<Args&&>(args)...);
	};
	std::apply(apply_this,t);
	return ans;
    }
};

template<class A, class B>
struct std::hash<std::pair<A,B> > {
    std::size_t operator()(std::pair<A,B> const& p) const {
	std::size_t ans = 0;
	my_hash_combine(ans,p.first);
	my_hash_combine(ans,p.second);
	return ans;
    }
};

template<class... Args>
struct std::hash<std::map<Args...>> {
    std::size_t operator()(std::map<Args...> const& m) const {
	std::size_t ans = 0;
	for (auto const& [x, y] : m) {
	    my_hash_combine(ans, x);
	    my_hash_combine(ans, y);
	}
	return ans;
    }
};

