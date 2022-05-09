#include "../combinations.hpp"
#include "harness.hpp"

TEST(choose, compileTime) {
    static_assert(constChoose<20, 10> == 184756, "Oops");
    static_assert(constChoose<40, 20> == 137846528820UL, "Oops");
    static_assert(constChoose<40, 50> == 0, "Oops");
    static_assert(constChoose<70, 10> == 396704524216UL, "Oops");
    static_assert(constChoose<70, 60> == 396704524216UL, "Oops");
    static_assert(constChoose<0, 0> == 1, "Oops");
}

TEST(choose, runtime) {
    CHECK(choose(20, 10), equals(184756U));
    CHECK(choose(40, 20), equals(137846528820UL));
    CHECK(choose(40, 50), equals(0U));
    CHECK(choose(70, 10), equals(396704524216UL));
    CHECK(choose(70, 60), equals(396704524216UL));
    CHECK(choose(0, 0), equals(1U));
}

TEST(choose, runtimeMemoized) {
    MemoizedChoose choose;
    CHECK(choose(20, 10), equals(184756U));
    CHECK(choose(40, 20), equals(137846528820UL));
    CHECK(choose(40, 50), equals(0U));
    CHECK(choose(70, 10), equals(396704524216UL));
    CHECK(choose(70, 60), equals(396704524216UL));
    CHECK(choose(0, 0), equals(1U));
}

TEST(permutations, compileTime) {
    static_assert(constPermutations<20> == 2432902008176640000UL, "Oops");
    static_assert(constPermutations<0> == 1, "Oops");
    static_assert(constPermutations<4> == 24, "Oops");
    static_assert(constPermutations<6> == 720, "Oops");
    static_assert(constPermutations<10> == 3628800, "Oops");
}

TEST(permutation, runtime) {
    CHECK(permutations(20), equals(2432902008176640000UL));
    CHECK(permutations(0), equals(1U));
    CHECK(permutations(4), equals(24U));
    CHECK(permutations(6), equals(720U));
    CHECK(permutations(10), equals(3628800U));
}

