#include "../BetterRegex.hpp"
#include "harness.hpp"

using namespace regex;
using namespace regex::short_literals;

TEST(BetterRegex, AtLeast) {
    std::regex regex = AtLeast(3) * "()"_v;
    CHECK("()()", doesNotMatch(regex));
    CHECK("()()()",matches(regex));
    CHECK("()()()()()", matches(regex));

    regex = ".."_v * AtLeast(2);
    CHECK("..", doesNotMatch(regex));
    CHECK("abcd", doesNotMatch(regex));
    CHECK("....", matches(regex));
    CHECK("......",matches(regex));
}

TEST(BetterRegex, Between) {
    auto regex = Between(3,5) * "f"_v;
    CHECK("ff", doesNotMatch(regex));
    CHECK("fff", matches(regex));
    CHECK("ffff", matches(regex));
    CHECK("fffff", matches(regex));
    CHECK("ffffff", doesNotMatch(regex));
}

TEST(BetterRegex, Exactly) {
    auto regex = Exactly(4) * "|"_v;
    CHECK("|||", doesNotMatch(regex));
    CHECK("||||", matches(regex));
    CHECK("|||||", doesNotMatch(regex));

    regex = "**"_v * 3;
    CHECK("******", matches(regex));
    CHECK("aaaaaa", doesNotMatch(regex));
}

TEST(BetterRegex, Optional) {
    auto regex = optional * "abc"_v;
    CHECK("", matches(regex));
    CHECK("abc", matches(regex));
    CHECK("abcabc", doesNotMatch(regex));
}

TEST(BetterRegex, Groups) {
    
}

TEST(BetterRegex, Or) {
    
}

TEST(BetterRegex, Combined) {
    
}
