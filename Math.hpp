#pragma once

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "TypeUtils.hpp"

#include <cstdlib>

template<class T>
Make_u<T> my_abs(T const t);

// for primitive types - checks if it's a power of 2
template<class T>
bool is_pow_2(T const& t);

// my_gcd: always returns an unsigned value
template<class X, class Y>
Common_u<X,Y> my_gcd(X const x, Y const y);

// my_lcm: always returns an unsigned value
template<class X, class Y>
Common_u<X,Y> my_lcm(X const x, Y const y);

// my_pow: returns an object of type Base
template<class Base, class Exp>
Base my_pow(Base b, Exp e);

// mod_inv: computes z such that x*z = m*y+1 for some integer m satisfying 0 <= z < |y|.
// If there is no such z this throws
template<class X, class Y>
Common_u<X,Y> mod_inv(X const& x, Y const& y);

// mod_inv_and_gcd: computes z and g such that g=gcd(x,y) and (x/g)*z = m*(y/g)+1 for some integer m satisfying 0 <= z < |y|/g.
template<class X, class Y>
std::pair<Common_u<X,Y>, Common_u<X,Y>> mod_inv_and_gcd(X const& x, Y const& y);

unsigned long factorial(unsigned long x);

unsigned int my_sqrt(unsigned long x);



// template impl

template<class T>
Make_u<T> my_abs(T const t) {
    using R = Make_u<T>;
    return t < T(0) ? static_cast<R>(-t) : static_cast<R>(t);
}

template<class T>
bool is_pow_2(T const& t) {
    return t > T(0) && (t & (t-T(1))) == T(0);
}

template<class X, class Y>
Common_u<X,Y> my_gcd(X const x, Y const y) {
    using U = Common_u<X,Y>;
    U a = my_abs<X>(x);
    U b = my_abs<Y>(y);
    if (!a)
	return b;
    if (!b)
	return a;
    for (U c(0); bool(c = a % b); ) {
	a = b;
	b = c;
    }
    return b;
}

template<class X, class Y>
Common_u<X,Y> my_lcm(X const x, Y const y) {
    using U = Common_u<X,Y>;
    U a = my_abs<X>(x);
    U b = my_abs<Y>(y);
    if (!a && !b)
	return U(0);
    return a/my_gcd(a,b)*b;
}

template<class Base, class Exp>
Base my_pow(Base b, Exp e) {
    Base result(1);
    Exp two(2);
    for (; e; e /= two) {
	if (e%two)
	    result *= b;
	b *= b;
    }
    return result;
}

template<class X, class Y>
Common_u<X,Y> mod_inv(X const& x, Y const& y) {
    auto [z, g] = mod_inv_and_gcd(x, y);
    
    if (g != 1) {
	throw_exception<std::invalid_argument>(
	    "non-coprime inputs " +
	    VAR_STR(x) +
	    " and " +
	    VAR_STR(y));
    }

    return z;
}

template<class X, class Y>
std::pair<Common_u<X,Y>, Common_u<X,Y>> mod_inv_and_gcd(X const& x, Y const& y) {
    using U = Common_u<X,Y>;

    if (y == Y(0))
	throw_exception<std::invalid_argument>("mod = 0");

    U mod = my_abs<Y>(y);
    U b = mod;
    U a = my_abs<X>(x) % b;
    
    U prev[2]{0,1};
    B negative[2]{false,false};

    for (U c; (c = a % b) > 0; ) {
	a /= b;
	
        U current;
	B current_negative;
	
	// older - div * last
	// prev[1] - div * prev[0]
	// two terms in the sum, so whichever is bigger carries the sign
	if (prev[1] > a * prev[0]) {
	    current = prev[1];
	    current = negative[1] == negative[0] ?
		current - a * prev[0] :
		current + a * prev[0];
	    
	    current_negative = negative[1];
	} else {
	    current = a * prev[0];
	    current = negative[1] == negative[0] ?
		current - prev[1] :
		current + prev[1];
	    
	    current_negative = !negative[0];
	}
	
	// shift previous values
	prev[1] = prev[0];
	negative[1] = negative[0];
	prev[0] = current;
	negative[0] = current_negative;
	
	a = b;
	b = c;
    }
    
    auto z = prev[0] ? negative[0]^(x<X(0)) ? mod-prev[0] : prev[0] : 0;
    auto g = b;

    return {z, g};
}

