#include "../countWithSmallPrimeFactors.hpp"
#include "harness.hpp"

#include "../PrimeUtils.hpp"
#include "../Now.hpp"

#include <set>

TEST(countLeqNwithPrimeFactorsLeqP, small) {
    CHECK(countLeqNwithPrimeFactorsLeqP(0, 2), equals(0U));
    CHECK(countLeqNwithPrimeFactorsLeqP(5, 2), equals(3U)); // 1, 2, 4
    CHECK(countLeqNwithPrimeFactorsLeqP(13, 3), equals(8U)); // 1, 2, 3, 4, 6, 8, 9, 12
}

TEST(countLeqNwithPrimeFactorsLeqP, bigButEasy) {
    // If N ≤ P, all 1 ≤ x ≤ N are possible
    CHECK(countLeqNwithPrimeFactorsLeqP(987'654'321, 999'888'777), equals(987'654'321U));
}

static UL dummySlowImpl(UL N, UI P) {
    if (N == 0) {
	return 0;
    }
    std::set<UL> possible{1};
    for (UI p : primesUpTo(P)) {
	decltype(possible) next;
	for (auto x : possible) {
	    for ( ; x <= N; x *= p) {
		next.insert(x);
	    }
	}
	std::swap(possible, next);
    }
    return possible.size();
}

TEST(countLeqNwithPrimeFactorsLeqP, medium) {
    UL N = static_cast<UL>(1e14);
    UI P = 5;
    
    CHECK(countLeqNwithPrimeFactorsLeqP(N, P), equals(dummySlowImpl(N, P)));
}

TEST(countLeqNwithPrimeFactorsLeqP, big) {
    UL N = static_cast<UL>(1e10);
    UI P = static_cast<UI>(1e5);

    CHECK(countLeqNwithPrimeFactorsLeqP(N, P), equals(3265474310U));
}

