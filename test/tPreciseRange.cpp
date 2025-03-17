#include "../PreciseRange.hpp"
#include "harness.hpp"

static bool eq(PreciseRange const& x, PreciseRange const& y) {
    return eq(x, y, -100);
}

static bool lt(PreciseRange const& x, PreciseRange const& y) {
    return cmp(x, y, -100) == PreciseRange::Cmp::Less;
}

static bool gt(PreciseRange const& x, PreciseRange const& y) {
    return cmp(x, y, -100) == PreciseRange::Cmp::Greater;
}

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
    // */
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

TEST(PreciseRange, ToStringExact) {
    CHECK(sqrt(-(3 * PreciseRange::pi()) + -4 / -PreciseRange(7) - PreciseRange("0.7") + PreciseRange("0xFF")).toStringExact(), equals("√(((-(3*π)+-4/-(7))-7/10)+255)"));
    CHECK(distanceToNearestInteger(exp(sin(cos(sinh(cosh(PreciseRange(5))))))).toStringExact(), equals("distanceToNearestInteger(e^(sin(cos(sinh(cosh(5))))))"));
}

TEST(PreciseRange, Comparison) {
    // All combinations of positives / negatives
    CHECK(lt(PreciseRange(3), PreciseRange(4)), isTrue());
    CHECK(lt(PreciseRange(-3), PreciseRange(-4)), isFalse());
    CHECK(lt(PreciseRange(3), PreciseRange(-4)), isFalse());
    CHECK(lt(PreciseRange(-3), PreciseRange(4)), isTrue());
}

TEST(PreciseRange, HighPrecisionAddition) {
    CHECK(eq(PreciseRange("1.2") + PreciseRange("1.4"), PreciseRange("2.6")), isTrue());
}

TEST(PreciseRange, HighPrecisionSubtraction) {
    CHECK(eq(PreciseRange(4) - PreciseRange("1.4"), PreciseRange("2.6")), isTrue());
}

TEST(PreciseRange, SubtractionAndPrinting) {
    // Verifies we eliminate all 0 decimals and the decimal point when a result is x.00
    CHECK((PreciseRange("0b1.1") - PreciseRange("0b0.1")).toStringWithUncertaintyLog2AtMost(0), equals("1"));
    // Verifies we eliminate trailing 0s when a result is x.y0
    CHECK((PreciseRange("0b1.11") - PreciseRange("0b0.01")).toStringWithUncertaintyLog2AtMost(0), equals("1.5"));
}

TEST(PreciseRange, HighPrecisionMultiplication) {
    CHECK(eq(PreciseRange(3) * PreciseRange("1.2"), PreciseRange("3.6")), isTrue());
}

TEST(PreciseRange, HighPrecisionDivision) {
    CHECK(lt(PreciseRange(9999) / 10000, PreciseRange(10000) / 10001), isTrue());
    // Note: decimal must be at least 2^(-100) ≈ 10^(-30) away from real value,
    //   since gt/lt use uncertainty 2^-100
    CHECK(gt(PreciseRange(7) / 3, PreciseRange("2.33333333333333333333333333333")), isTrue());
    CHECK(lt(PreciseRange(7) / 3, PreciseRange("2.33333333333333333333333333334")), isTrue());
}

TEST(PreciseRange, DivisionBy0) {
    // doesn't throw on construction
    CHECK((PreciseRange(1) / PreciseRange(0), true), isTrue());
    CHECK((PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")), true), isTrue());
    // throws when division is evaluated
    /* this makes the test too slow
    CHECK_THROWS(gt(PreciseRange(1) / PreciseRange(0), PreciseRange(0)), std::domain_error);
    CHECK_THROWS(gt(PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")), PreciseRange(0)), std::domain_error);
    // */
}

TEST(PreciseRange, DistanceToNearestInteger) {
    // critical + exact:
    CHECK(eq(distanceToNearestInteger(PreciseRange(0)), PreciseRange(0)), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(eq(distanceToNearestInteger(PreciseRange(6)), PreciseRange(0)), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange(6)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(eq(distanceToNearestInteger(PreciseRange(-6)), PreciseRange(0)), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange(-6)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(eq(distanceToNearestInteger(PreciseRange("0b11.1")), PreciseRange("0b.1")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("0b11.1")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.5"));
    CHECK(eq(distanceToNearestInteger(PreciseRange("-0b11.1")), PreciseRange("0b.1")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("0b11.1")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.5"));

    // non-critical + exact:
    CHECK(eq(distanceToNearestInteger(PreciseRange("0b10.001")), PreciseRange("0b.001")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("0b10.001")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.125"));
    CHECK(eq(distanceToNearestInteger(PreciseRange("-0b10.001")), PreciseRange("0b.001")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("-0b10.001")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.125"));
    CHECK(eq(distanceToNearestInteger(PreciseRange("0b10.1101")), PreciseRange("0b.0011")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("0b10.1101")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.1875"));
    CHECK(eq(distanceToNearestInteger(PreciseRange("-0b10.1101")), PreciseRange("0b.0011")), isTrue());
    CHECK(distanceToNearestInteger(PreciseRange("-0b10.1101")).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.1875"));

    // critical + inexact:
    CHECK(eq(distanceToNearestInteger(PreciseRange("5.1") - PreciseRange("5.1")), PreciseRange(0)), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("1.1") - PreciseRange("7.1")), PreciseRange(0)), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("7.1") - PreciseRange("1.1")), PreciseRange(0)), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("1.6") - PreciseRange("7.1")), PreciseRange("0b.1")), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("7.1") - PreciseRange("1.6")), PreciseRange("0b.1")), isTrue());

    // non-critical + inexact:
    CHECK(eq(distanceToNearestInteger(PreciseRange("2.1")), PreciseRange("0.1")), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("-3.1")), PreciseRange("0.1")), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("5.9")), PreciseRange("0.1")), isTrue());
    CHECK(eq(distanceToNearestInteger(PreciseRange("-6.9")), PreciseRange("0.1")), isTrue());

    // large uncertainty - rounds endpoints to [0, 0.5]
    CHECK(distanceToNearestInteger(PreciseRange("2") / PreciseRange("0.01")).toStringWithUncertaintyLog2AtMost(9), equals("[0.25 ± 0.25]"));

    // Check when very close to 0 that we're above 0
    CHECK(gt(distanceToNearestInteger(PreciseRange("0.00000000000000000001")), PreciseRange(0)), isTrue());
}
