#include "countWithSmallPrimeFactors.hpp"

#include "O_Map.hpp"
#include "PrimeUtils.hpp"

UL countLeqNwithPrimeFactorsLeqP(UL N, UI P) {
    if (N == 0) {
	return 0;
    }
    if (N <= P) {
	return N;
    }
    auto primes = primesUpTo(P);
    
    // More overlap if we start from high primes -> faster
    std::reverse(primes.begin(), primes.end());
    
    // std::greater to iterate high-to-low, since when adding in a prime p,
    //   x -> x, x/p, x/p^2, x/p^3, ...
    // And iterating starting from x, we add count[x] -> count[x/p] -> count[x/p^2] -> ...
    O_Map<UL, UL, std::greater<>> counts{{N, 1}};
    {
	for (auto p : primes) {
	    for (auto const& [x, count] : counts) {
		if (x < p) {
		    break;
		}
		counts[x / p] += count;
	    }
	}
    }
    UL total = 0;
    for (auto const& [x, count] : counts) {
	total += count;
    }
    return total;
}

