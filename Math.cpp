#include "Math.hpp"
#include "SafeMath.hpp"

#include <cmath>

unsigned long factorial(unsigned long x) {
    SafeMath<unsigned long> result(1U);
    for (; x>0; x--)
	result *= x;
    return *result;
}

unsigned int my_sqrt(unsigned long x) {
    UL r = static_cast<UL>(std::sqrt(x));
    // std::sqrt casts to double, possibly losing precision, so check above and below to be safe
    while (r*r > x) {
	--r;
    }
    while (static_cast<UI>(r+1) && (r+1)*(r+1) <= x) {
	++r;
    }
    return static_cast<UI>(r);
}

