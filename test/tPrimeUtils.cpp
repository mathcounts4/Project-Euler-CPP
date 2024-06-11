#include "../PrimeUtils.hpp"
#include "harness.hpp"

TEST(primitiveRoot, small) {
    CHECK(primitiveRoot(2), equals(1u));
    CHECK(primitiveRoot(3), equals(2u));
    CHECK(primitiveRoot(5), equals(2u));
    CHECK(primitiveRoot(7), equals(3u));
    CHECK(primitiveRoot(11), equals(2u));
    CHECK(primitiveRoot(13), equals(2u));
    CHECK(primitiveRoot(17), equals(3u));
    CHECK(primitiveRoot(19), equals(2u));
    CHECK(primitiveRoot(23), equals(5u));
    CHECK(primitiveRoot(29), equals(2u));
    CHECK(primitiveRoot(31), equals(3u));
    CHECK(primitiveRoot(37), equals(2u));
    CHECK(primitiveRoot(41), equals(6u));
    CHECK(primitiveRoot(43), equals(3u));
    CHECK(primitiveRoot(47), equals(5u));
}

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
