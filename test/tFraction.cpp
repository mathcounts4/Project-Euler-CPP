#include "../Fraction.hpp"
#include "harness.hpp"

TEST(Fraction,bug) {
    CHECK(Fraction(-5)/2, equals(Fraction(5,-2)));
}

