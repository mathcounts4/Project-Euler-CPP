#include "../PrimeUtils.hpp"
#include "harness.hpp"

TEST(sqrtModPrime, two) {
    CHECK(*sqrtModPrime(-77, 2), equals(1U));
    CHECK(*sqrtModPrime(77, 2), equals(1U));
    CHECK(*sqrtModPrime(-78, 2), equals(0U));
    CHECK(*sqrtModPrime(78, 2), equals(0U));
}

TEST(sqrtModPrime, three) {
    CHECK(*sqrtModPrime(0, 3), equals(0U));
    CHECK(*sqrtModPrime(1, 3), equals(1U));
    CHECK(sqrtModPrime(2, 3), isFalse());
}

TEST(sqrtModPrime, _101) {
    UI p = 101;
    UI squares = 0;
    for (UI i = 0; i < p; ++i) {
	if (auto rt = sqrtModPrime(i, p)) {
	    ++squares;
	    CHECK(*rt * *rt % p, equals(i));
	}
    }
    CHECK(squares, equals((p+1)/2));
}

TEST(sqrtModPrime, _103) {
    UI p = 103;
    UI squares = 0;
    for (UI i = 0; i < p; ++i) {
	if (auto rt = sqrtModPrime(i, p)) {
	    ++squares;
	    CHECK(*rt * *rt % p, equals(i));
	}
    }
    CHECK(squares, equals((p+1)/2));
}
