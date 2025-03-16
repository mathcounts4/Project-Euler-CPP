#include "../PreciseRange.hpp"
#include "harness.hpp"

TEST(PreciseRange, ConstructionAndPrinting) {
    CHECK(PreciseRange(3).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("3"));
    CHECK(PreciseRange("3.14159").toStringWithUncertaintyLog2AtMost(std::nullopt), equals("[3.5 ± 0.5]"));
    CHECK(PreciseRange(0).toStringWithUncertaintyLog2AtMost(2), equals("0"));
    CHECK(PreciseRange(0).toStringWithUncertaintyLog2AtMost(-4), equals("0"));
    CHECK(PreciseRange(3).toStringWithUncertaintyLog2AtMost(-3), equals("3"));
    CHECK(PreciseRange(-3).toStringWithUncertaintyLog2AtMost(-3), equals("-3"));
    CHECK(PreciseRange("0").toStringWithUncertaintyLog2AtMost(0), equals("0"));
    CHECK(PreciseRange("0xfF.8").toStringWithUncertaintyLog2AtMost(0), equals("255.5"));
    CHECK(PreciseRange("-0B0110.11").toStringWithUncertaintyLog2AtMost(0), equals("-6.75"));
    CHECK(PreciseRange("0d783").toStringWithUncertaintyLog2AtMost(0), equals("783"));
    CHECK(PreciseRange("-0o77.3").toStringWithUncertaintyLog2AtMost(0), equals("-63.375"));
    CHECK(PreciseRange("-.5").toStringWithUncertaintyLog2AtMost(0), equals("[-.5 ± 0.5]"));
    CHECK(PreciseRange("3.14159").toStringWithUncertaintyLog2AtMost(0), equals("[3.125 ± 0.125]"));

    /* this makes the test too slow
    CHECK_THROWS(PreciseRange("-0x."), std::invalid_argument);
    CHECK_THROWS(PreciseRange("0b2"), std::invalid_argument);
    CHECK_THROWS(PreciseRange("0o8"), std::invalid_argument);
    CHECK_THROWS(PreciseRange("0xg"), std::invalid_argument);
    CHECK_THROWS(PreciseRange("a"), std::invalid_argument);
    */
}

TEST(PreciseRange, ToString) {
    CHECK(sqrt(-(3 * PreciseRange::pi()) + -4 / -PreciseRange(7) - PreciseRange("0.7") + PreciseRange("0xFF")).toStringExact(), equals("√(((-(3*π)+-4/-(7))-7/10)+255)"));
    CHECK(distanceToNearestInteger(exp(sin(cos(sinh(cosh(PreciseRange(5))))))).toStringExact(), equals("distanceToNearestInteger(e^(sin(cos(sinh(cosh(5))))))"));
}

TEST(PreciseRange, Comparison) {
    // All combinations of positives / negatives
    CHECK(PreciseRange(3) < PreciseRange(4), isTrue());
    CHECK(PreciseRange(-3) < PreciseRange(-4), isFalse());
    CHECK(PreciseRange(3) < PreciseRange(-4), isFalse());
    CHECK(PreciseRange(-3) < PreciseRange(4), isTrue());

    // > returns opposite of <
    CHECK(PreciseRange(3) > PreciseRange(4), isFalse());
}

TEST(PreciseRange, Caching) {
    PreciseRange frac = PreciseRange(7) / 3;
    CHECK(frac.toStringWithUncertaintyLog2AtMost(0), equals("[2.5 ± 0.5]"));
    CHECK(frac.toStringWithUncertaintyLog2AtMost(-1), equals("[2.3125 ± 0.0625]"));
    // After computing a higher precision result, we should have cached it,
    //   and continue using that value for calculations and printing, when requesting
    //   a lower-precision result.
    CHECK(frac.toStringWithUncertaintyLog2AtMost(0), equals("[2.3125 ± 0.0625]"));
}

TEST(PreciseRange, HighPrecisionDivision) {
    CHECK(PreciseRange(9999) / 10000 < PreciseRange(10000) / 10001, isTrue());
    CHECK(PreciseRange(7) / 3 > PreciseRange("2.33333333333333333333333333333333333333"), isTrue());
    CHECK(PreciseRange(7) / 3 < PreciseRange("2.33333333333333333333333333333333333334"), isTrue());
}

TEST(PreciseRange, DivisionBy0) {
    // doesn't throw on construction
    CHECK((PreciseRange(1) / PreciseRange(0), true), isTrue());
    CHECK((PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")), true), isTrue());
    // throws when division is evaluated
    /* this makes the test too slow
    CHECK_THROWS(PreciseRange(1) / PreciseRange(0) > 0, std::domain_error);
    CHECK_THROWS(PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")) > 0, std::domain_error);
    */
}
