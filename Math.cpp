#include "Math.hpp"
#include "SafeMath.hpp"

unsigned long factorial(unsigned long x) {
    SafeMath<unsigned long> result(1U);
    for (; x>0; x--)
	result *= x;
    return *result;
}

