/* This creates a std::hash template type for your custom class

In the custom class file:

1. Include this header
2. In your class definition, place the following line:
    friend std::size_t std::hash<MyClass>::operator()(const MyClass & x) const;
3. After your class definition, use one of the defined macros:
3a. If using a non-template custom class:
    MAKE_HASHABLE(MyClass,x,x.member1,x.member2,...)
3b. If using a template custom class:
    MAKE_HASHABLE_TEMPLATE(<class A, class B>,MyClass<A,B>,x,x.member1,...)
*/

#pragma once
#ifndef MY_HASH_UTIL
#define MY_HASH_UTIL

#define MAKE_HASHABLE_TEMPLATE(hashTypes,Class,obj,...) \
    template hashTypes					\
    struct std::hash<Class> {				\
	std::size_t operator()(Class const & obj) const {	\
	    std::size_t ans = 0;				\
	    my_hash_combine(ans,__VA_ARGS__);		\
	    return ans;					\
	}						\
    };

#define MAKE_HASHABLE(Class,obj,...)		\
    MAKE_HASHABLE_TEMPLATE(<>,Class,obj,...)

inline void my_hash_combine(std::size_t &) {}

template<class T, class... Classes>
inline void my_hash_combine(std::size_t & seed,
			 T const & t,
			 Classes... args) {
    seed ^= std::hash<T>()(t) + 0x9e3779b97f4a7c16 + (seed<<6) + (seed>>2);
    my_hash_combine(seed,args...);
}

template<class T>
struct std::hash {
    std::size_t operator()(T const & t) const {
	return std::hash<int>()(int(t));
    }
};

#endif /* MY_HASH_UTIL */
