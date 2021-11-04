#include "../SafeMath.hpp"
#include "harness.hpp"

static void init() {
    SafeMath_failFast = false;
}

TEST(SafeMathNegative,cast) {
    init();
    CHECK(SafeMath<unsigned int>::cast(-3).ok(), isFalse());
    CHECK(SafeMath<unsigned int>::cast(33485763485235UL).ok(), isFalse());
    CHECK(SafeMath<int>::cast(33485763485235UL).ok(), isFalse());
}

TEST(SafeMathNegative,plus) {
    init();
    auto constexpr uint_max = std::numeric_limits<unsigned int>::max();
    auto constexpr int_min = std::numeric_limits<int>::lowest();
    
    SafeMath<unsigned int> safe_uint_max{uint_max};
    SafeMath<int> safe_int_min{int_min};
    
    CHECK((SafeMath<unsigned int>{} + (-3)).ok(), isFalse());
    CHECK((SafeMath<unsigned int>{} + (-3)).ok(), isFalse());
    CHECK((safe_int_min + (-1)).ok(), isFalse());
    CHECK((SafeMath<int>{-1} + int_min).ok(), isFalse());
    
    CHECK((safe_uint_max + static_cast<signed char>(1)).ok(), isFalse()); // smaller
    CHECK((safe_uint_max + 1U).ok(), isFalse()); // same size
    CHECK((safe_uint_max + 1).ok(), isFalse()); // same size
    CHECK((safe_int_min + 3U).ok(), isFalse()); // same size
    CHECK((safe_int_min + 3UL).ok(), isFalse()); // bigger
}

TEST(SafeMathNegative,times) {
    CHECK((SafeMath<unsigned int>{1U} * (-3)).ok(), isFalse());
}

