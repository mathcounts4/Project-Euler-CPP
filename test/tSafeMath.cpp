#include "../SafeMath.hpp"
#include "harness.hpp"

TEST(SafeMath,construct) {
    CHECK(SafeMath<int>(5), equals(5));
    CHECK(SafeMath<int>(-5), equals(-5));
}

TEST(SafeMath,plus) {
    CHECK(SafeMath<int>(5) + 7, equals(12));
    CHECK(SafeMath<int>(-5) + 7, equals(2));
    CHECK(SafeMath<int>(5) + (-7), equals(-2));
    CHECK(SafeMath<int>(-5) + (-7), equals(-12));
}

TEST(SafeMath,plusBoundary) {
    auto constexpr uint_max = std::numeric_limits<unsigned int>::max();
    auto constexpr int_min = std::numeric_limits<int>::lowest();
    long const beyond_int = long(int_min) - 1L;
    
    SafeMath<unsigned int> safe_uint_max{uint_max};
    SafeMath<int> safe_int_min{int_min};
    
    CHECK(safe_uint_max + static_cast<signed char>(-3), equals(uint_max-3U)); // smaller
    CHECK(safe_uint_max + (-3), equals(uint_max-3U)); // same size
    CHECK(safe_uint_max + (-3L), equals(uint_max-3U)); // bigger
    
    CHECK(safe_int_min + static_cast<unsigned char>(3), equals(int_min+3)); // smaller

    CHECK(SafeMath<int>{1} + beyond_int, equals(int_min));
}

TEST(SafeMath,plusExpand) {
    auto constexpr uint_max = std::numeric_limits<unsigned int>::max();
    unsigned long const beyond_uint = uint_max + 1UL;
    
    SafeMath<unsigned int> safe_uint_max{uint_max};
    
    CHECK(safe_uint_max + 1L, equals(beyond_uint));
    CHECK(safe_uint_max + 1UL, equals(beyond_uint));
}

TEST(SafeMath,minus) {
    CHECK(SafeMath<int>(5) - 7, equals(-2));
    CHECK(SafeMath<int>(-5) - 7, equals(-12));
    CHECK(SafeMath<int>(5) - (-7), equals(12));
    CHECK(SafeMath<int>(-5) - (-7), equals(2));
}

TEST(SafeMath,times) {
    CHECK(SafeMath<int>(5) * 7, equals(35));
    CHECK(SafeMath<int>(5) * -7, equals(-35));
    CHECK(SafeMath<int>(-5) * 7, equals(-35));
    CHECK(SafeMath<int>(-5) * -7, equals(35));
}

TEST(SafeMath,divide) {
    CHECK(SafeMath<int>(21) / 7, equals(3));
    CHECK(SafeMath<int>(27) / 7, equals(3));
    CHECK(SafeMath<int>(-21) / 7, equals(-3));
    CHECK(SafeMath<int>(-27) / 7, equals(-3));
    CHECK(SafeMath<int>(21) / -7, equals(-3));
    CHECK(SafeMath<int>(27) / -7, equals(-3));
    CHECK(SafeMath<int>(-21) / -7, equals(3));
    CHECK(SafeMath<int>(-27) / -7, equals(3));
}

