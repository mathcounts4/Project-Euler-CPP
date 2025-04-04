#include "../PreciseRange.hpp"
#include "harness.hpp"

static bool eq(PreciseRange const& x, PreciseRange const& y) {
    return eq(x, y, -100);
}

static bool lt(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2 = -100) {
    return cmp(x, y, maxUncertaintyLog2) == PreciseRange::Cmp::Less;
}

static bool gt(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2 = -100) {
    return cmp(x, y, maxUncertaintyLog2) == PreciseRange::Cmp::Greater;
}

static bool strictRange(PreciseRange const& low,
			PreciseRange const& value,
			PreciseRange const& high,
			std::int64_t maxUncertaintyLog2 = -100) {
    return gt(value, low, maxUncertaintyLog2) && lt(value, high, maxUncertaintyLog2);
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
    CHECK(PreciseRange("-.5").toStringWithUncertaintyLog2AtMost(0), equals("[-0.5 ± 0.5]"));
    CHECK(PreciseRange("3.14159").toStringWithUncertaintyLog2AtMost(0), equals("[3.5 ± 0.5]"));
}

TEST(PreciseRange, ConstructionInvalid) {
    if (shouldRunSlowTests()) {
	CHECK_THROWS(PreciseRange("-0x."), std::invalid_argument);
	CHECK_THROWS(PreciseRange("0b2"), std::invalid_argument);
	CHECK_THROWS(PreciseRange("0o8"), std::invalid_argument);
	CHECK_THROWS(PreciseRange("0xg"), std::invalid_argument);
	CHECK_THROWS(PreciseRange("a"), std::invalid_argument);
    }
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
    CHECK(sqrt(-(3 * PreciseRange::pi()) + -4 / -PreciseRange(7) - PreciseRange("0.7") + PreciseRange("0xFF")).toStringExact(), equals("√(((-(3*π)+-4/(-(7)))-7/10)+255)"));
    CHECK(distanceToNearestInteger(ln(exp(sin(cos(sinh(cosh(PreciseRange(5)))))))).toStringExact(), equals("distanceToNearestInteger(ln(e^(sin(cos(sinh(cosh(5)))))))"));
    CHECK((2 * mod(PreciseRange(44) * PreciseRange(7), PreciseRange(5) - PreciseRange(2)) - 3).toStringExact(), equals("2*((44*7) mod (5-2))-3"));
    CHECK((((PreciseRange(2) * 3) * (PreciseRange(4) * 5)) ^ -6).toStringExact(), equals("((2*3)*(4*5))^-6"));
    CHECK((((PreciseRange(2) + 3) >> 4 << 5) + 6).toStringExact(), equals("(((2+3)>>4)<<5)+6"));
    CHECK((((PreciseRange(2) * 3) << 4) * 5).toStringExact(), equals("(2*3<<4)*5"));
    CHECK(((PreciseRange(2) * 3) ^ -6).toStringExact(), equals("(2*3)^-6"));
    CHECK(((PreciseRange(2) + 3) ^ -6).toStringExact(), equals("(2+3)^-6"));
    CHECK((PreciseRange(-2) ^ -6).toStringExact(), equals("-2^-6"));
    CHECK(((-PreciseRange(2)) ^ -6).toStringExact(), equals("(-(2))^-6"));
    CHECK((-(PreciseRange(2) ^ -6)).toStringExact(), equals("-(2^-6)"));
}

TEST(PreciseRange, Comparison) {
    // All combinations of positives / negatives
    CHECK(lt(PreciseRange(3), PreciseRange(4)), isTrue());
    CHECK(lt(PreciseRange(-3), PreciseRange(-4)), isFalse());
    CHECK(lt(PreciseRange(3), PreciseRange(-4)), isFalse());
    CHECK(lt(PreciseRange(-3), PreciseRange(4)), isTrue());
}

TEST(PreciseRange, Pi) {
    CHECK(PreciseRange::pi().toStringWithUncertaintyLog2AtMost(std::nullopt), equals("[3.125 ± 0.125]"));
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

TEST(PreciseRange, DropUnnecessaryExtraBits) {
    // This test verifies we drop extra unnecessary bits after performing a calculation.
    // For example, [1,2] * [4, 4.5] = [4, 9].
    // [4, 9] has uncertainty 5, and ⌈log2(uncertainty)⌉ = 3.
    // We might as well round to produce [4, 12], which has the same uncertainty,
    //   but requires storage of fewer bits, which corresponds to a cheaper cost of
    //   downstream operations.
    CHECK(sqrt(PreciseRange("10") * PreciseRange("0.1")).toStringWithUncertaintyLog2AtMost(1), equals("[1 ± 0.25]"));

    CHECK((PreciseRange(1) / 9 * 5).toStringWithUncertaintyLog2AtMost(1), equals("[0.5625 ± 0.125]"));
}

TEST(PreciseRange, HighPrecisionDivision) {
    CHECK(lt(PreciseRange(9999) / 10000, PreciseRange(10000) / 10001), isTrue());
    // Note: decimal must be at least 2^(-100) ≈ 10^(-30) away from real value,
    //   since gt/lt use uncertainty 2^-100
    CHECK(strictRange(PreciseRange("2.33333333333333333333333333333"), PreciseRange(7) / 3, PreciseRange("2.33333333333333333333333333334")), isTrue());
}

TEST(PreciseRange, DivisionBy0) {
    // doesn't throw on construction
    CHECK((PreciseRange(1) / PreciseRange(0), true), isTrue());
    CHECK((PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")), true), isTrue());
    if (shouldRunSlowTests()) {
	// throws when division is evaluated
	CHECK_THROWS(gt(PreciseRange(1) / PreciseRange(0), PreciseRange(0)), std::domain_error);
	CHECK_THROWS(gt(PreciseRange(1) / (PreciseRange("0.3") - PreciseRange("0.3")), PreciseRange(0)), std::domain_error);
    }
}

TEST(PreciseRange, Mod) {
    CHECK(mod(PreciseRange(77), PreciseRange(7)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(mod(PreciseRange(78), PreciseRange(7)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-6"));
    CHECK(mod(PreciseRange(-77), PreciseRange(7)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(mod(PreciseRange(-78), PreciseRange(7)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-1"));

    auto eqOneOf = [](auto const& x, auto const& a, auto const& b) {
	return eq(x, a) || eq(x, b);
    };

    // 387/14 mod 4/7 = 3/14, so mod should converge to 3/14 or -5/14
    CHECK(eqOneOf(mod(PreciseRange(387) / 14, PreciseRange(4) / 7), PreciseRange(3) / 14, PreciseRange(-5) / 14), isTrue());

    // 387/14 mod -4/7 = 3/14, so mod should converge to 3/14 or -5/14
    CHECK(eqOneOf(mod(PreciseRange(387) / 14, PreciseRange(-4) / 7), PreciseRange(3) / 14, PreciseRange(-5) / 14), isTrue());

    // -387/14 mod 4/7 = -3/14, so mod should converge to -3/14 or 5/14
    CHECK(eqOneOf(mod(PreciseRange(-387) / 14, PreciseRange(4) / 7), PreciseRange(-3) / 14, PreciseRange(5) / 14), isTrue());

    // -387/14 mod -4/7 = -3/14, so mod should converge to -3/14 or 5/14
    CHECK(eqOneOf(mod(PreciseRange(-387) / 14, PreciseRange(-4) / 7), PreciseRange(-3) / 14, PreciseRange(5) / 14), isTrue());
}

TEST(PreciseRange, Power) {
    CHECK((PreciseRange("2.7") ^ 0).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("1"));
    CHECK((PreciseRange(3) ^ 5).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("243"));
    CHECK(eq(PreciseRange("1.1") ^ 3, "1.331"), isTrue());
    CHECK(eq((PreciseRange(2) / 3) ^ -2, "2.25"), isTrue());
    CHECK(eq(PreciseRange("-1.003") ^ 22222, PreciseRange("1.006009") ^ 11111), isTrue());
}

TEST(PreciseRange, Shift) {
    CHECK((PreciseRange(3) << 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("12"));
    CHECK((PreciseRange(-3) << 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-12"));
    CHECK((PreciseRange(12) << -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("3"));
    CHECK((PreciseRange(-12) << -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-3"));
    CHECK((PreciseRange(3) << -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.75"));
    CHECK((PreciseRange(-3) << -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-0.75"));

    CHECK((PreciseRange(3) >> -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("12"));
    CHECK((PreciseRange(-3) >> -2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-12"));
    CHECK((PreciseRange(12) >> 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("3"));
    CHECK((PreciseRange(-12) >> 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-3"));
    CHECK((PreciseRange(3) >> 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0.75"));
    CHECK((PreciseRange(-3) >> 2).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-0.75"));
    CHECK((PreciseRange(-3) >> 20).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("-0.00000286102294921875"));

    CHECK(eq(PreciseRange("0.1") << 3, "0.8"), isTrue());
    CHECK(eq(PreciseRange("-0.1") << 3, "-0.8"), isTrue());
    CHECK(eq(PreciseRange("0.125") >> -5, 4), isTrue());
}

TEST(PreciseRange, Sqrt) {
    // critical + exact:
    CHECK(eq(sqrt(PreciseRange(0)), PreciseRange(0)), isTrue());

    // non-critical + exact:
    CHECK(eq(sqrt(PreciseRange("0b10.01")), PreciseRange("0b1.1")), isTrue());

    // critical + inexact:
    CHECK(eq(sqrt(PreciseRange("5.1") - PreciseRange("5.1")), PreciseRange(0)), isTrue());

    // non-critical + inexact:
    CHECK(eq(sqrt(PreciseRange("2.1") + PreciseRange("0.15")), PreciseRange("1.5")), isTrue());

    // At least enough precision + exact input:
    CHECK(sqrt(PreciseRange(2)).toStringWithUncertaintyLog2AtMost(-333), matches(std::regex(regexEscape("[1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572") + ".*")));

    // At least enough precision + inexact input:
    CHECK(sqrt(PreciseRange("2.1") - PreciseRange("0.1")).toStringWithUncertaintyLog2AtMost(-333), matches(std::regex(regexEscape("[1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572") + ".*")));
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

TEST(PreciseRange, Exp) {
    CHECK(exp(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("1"));
    CHECK(eq(exp(PreciseRange("0.1") - PreciseRange("0.1")), PreciseRange(1)), isTrue());

    // e = 2.718281...
    CHECK(strictRange("2.718281", exp(PreciseRange(1)), "2.718282"), isTrue());
    CHECK(strictRange("2.718281", exp(PreciseRange("1.1") - PreciseRange("0.1")), "2.718282"), isTrue());

    // e^-1 = 0.367879...
    CHECK(strictRange("0.367879", exp(PreciseRange(-1)), "0.367880"), isTrue());
    CHECK(strictRange("0.367879", exp(PreciseRange("0.1") - PreciseRange("1.1")), "0.367880"), isTrue());

    // e^5 = 148.413159...
    CHECK(strictRange("148.413159", exp(PreciseRange(5)), "148.413160"), isTrue());
    CHECK(strictRange("148.413159", exp(PreciseRange("5.1") - PreciseRange("0.1")), "148.413160"), isTrue());

    // e^-5 = 0.006737946...
    CHECK(strictRange("0.006737946", exp(PreciseRange(-5)), "0.006737947"), isTrue());
    CHECK(strictRange("0.006737946", exp(PreciseRange("0.1") - PreciseRange("5.1")), "0.006737947"), isTrue());
}

TEST(PreciseRange, ExpLarge) {
    // e^200 = 7.2259738...e+86
    CHECK(strictRange("7.22597", exp(PreciseRange(200)) / (PreciseRange(10) ^ 86), "7.22598"), isTrue());
    CHECK(strictRange("7.22597", exp((PreciseRange("200.1") - PreciseRange("0.1"))) / (PreciseRange(10) ^ 86), "7.22598"), isTrue());

    // e^2000 = 3.88118019...e+868
    CHECK(strictRange("3.88118019", exp(PreciseRange(2000)) / (PreciseRange(10) ^ 868), "3.88118020"), isTrue());

    // e^10000 = 8.80681822...e+4342
    CHECK((exp(PreciseRange(10000)) / (PreciseRange(10) ^ 4342)).toStringWithUncertaintyLog2AtMost(-200), matches(std::regex(regexEscape("[8.806818225662921587261496007644561003520004085591508936424570") + ".*")));

    // TODO: see why e^1000000 is slow
}

TEST(PreciseRange, Ln) {
    CHECK(ln(PreciseRange(1)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(eq(ln(PreciseRange("1.1") - PreciseRange("0.1")), 0), isTrue());

    CHECK(eq(ln(exp(PreciseRange(1))), 1), isTrue());
    CHECK(eq(ln(exp(PreciseRange(-1))), -1), isTrue());

    CHECK(eq(ln(exp(PreciseRange(5))), 5), isTrue());
    CHECK(eq(ln(exp(PreciseRange(-5))), -5), isTrue());

    CHECK(eq(ln(exp(PreciseRange("2.345987"))), "2.345987"), isTrue());
    CHECK(eq(ln(exp(PreciseRange("-7.3495874"))), "-7.3495874"), isTrue());

    PreciseRange x("9.234956871");
    PreciseRange y("5.234501204");
    CHECK(eq(ln(x * y), ln(x) + ln(y)), isTrue());

    PreciseRange xSmall("0.0000000043495873");
    PreciseRange ySmall("0.0000000034598721");
    CHECK(eq(ln(xSmall * ySmall), ln(xSmall) + ln(ySmall)), isTrue());

    PreciseRange base("5.349587");
    // x^10 = e^(ln(x^10)) = e^(10*ln(x))
    CHECK(eq(base ^ 10, exp(10 * ln(base))), isTrue());

    // x^y = e^(ln(x^y)) = e^(y*ln(x))
    // 1.32495873 ^ 27.4395874 ≈ 2255.1845690
    CHECK(strictRange("2255.1845690", exp(PreciseRange("27.4395874") * ln(PreciseRange("1.32495873"))), "2255.1845691"), isTrue());
}

TEST(PreciseRange, LnInvalidInput) {
    // doesn't throw on construction
    CHECK((ln(PreciseRange(0)), true), isTrue());
    CHECK((ln(PreciseRange("0.3") - PreciseRange("0.3")), true), isTrue());
    CHECK((ln(PreciseRange(-1)), true), isTrue());
    CHECK((ln(PreciseRange("0.3") - PreciseRange("1.3")), true), isTrue());
    if (shouldRunSlowTests()) {
	// throws when ln is evaluated
	CHECK_THROWS(gt(ln(PreciseRange(0)), PreciseRange(0)), std::domain_error);
	CHECK_THROWS(gt(ln(PreciseRange("0.3") - PreciseRange("0.3")), PreciseRange(0)), std::domain_error);
	CHECK_THROWS(gt(ln(PreciseRange(-1)), PreciseRange(0)), std::domain_error);
	CHECK_THROWS(gt(ln(PreciseRange("0.3") - PreciseRange("1.3")), PreciseRange(0)), std::domain_error);
    }
}

TEST(PreciseRange, LnLarge) {
    CHECK(strictRange("4605.170185", ln(PreciseRange(10) ^ 2000), "4605.170186"), isTrue());
}

TEST(PreciseRange, SinCos) {
    CHECK(sin(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(cos(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("1"));
    CHECK(eq(sin(PreciseRange("0.1") - PreciseRange("0.1")), PreciseRange(0)), isTrue());
    CHECK(eq(cos(PreciseRange("0.1") - PreciseRange("0.1")), PreciseRange(1)), isTrue());

    // sin(0.3) = 0.295520...
    CHECK(strictRange("0.295520", sin(PreciseRange("0.3")), "0.295521"), isTrue());
    // cos(0.3) = 0.955336...
    CHECK(strictRange("0.955336", cos(PreciseRange("0.3")), "0.955337"), isTrue());

    // sin(2.3) = 0.745705...
    CHECK(strictRange("0.745705", sin(PreciseRange("2.3")), "0.745706"), isTrue());
    // cos(2.3) = -0.666276...
    CHECK(strictRange("-0.666277", cos(PreciseRange("2.3")), "-0.666276"), isTrue());

    // sin(35.5) = -0.8090187...
    CHECK(strictRange("-0.8090188", sin(PreciseRange("35.5")), "-0.8090187"), isTrue());
    // cos(35.5) = -0.587782...
    CHECK(strictRange("-0.587783", cos(PreciseRange("35.5")), "-0.587782"), isTrue());

    // sin(-19.3) = -0.4353653...
    CHECK(strictRange("-0.4353654", sin(PreciseRange("-19.3")), "-0.4353653"), isTrue());
    // cos(-19.3) = 0.9002538...
    CHECK(strictRange("0.9002538", cos(PreciseRange("-19.3")), "0.9002539"), isTrue());
}

TEST(PreciseRange, SinCosIdentities) {
    PreciseRange one(1);
    CHECK(eq(sin(one)*sin(one) + cos(one)*cos(one), one), isTrue());
    PreciseRange two(2);
    CHECK(eq(sin(two)*sin(two) + cos(two)*cos(two), one), isTrue());
    PreciseRange x("2.3456789");
    CHECK(eq(sin(x)*sin(x) + cos(x)*cos(x), one), isTrue());
    CHECK(eq(sin(-x), -sin(x)), isTrue());
    CHECK(eq(cos(-x), cos(x)), isTrue());
    CHECK(eq(sin(2 * x), 2 * sin(x) * cos(x)), isTrue());
    CHECK(eq(sin(x / 2), sqrt((1 - cos(x)) / 2)), isTrue());
}

TEST(PreciseRange, SinCosLarge) {
    // sin(300) = -0.9997558...
    CHECK(strictRange("-0.9997559", sin(PreciseRange(300)), "-0.9997558"), isTrue());
    // cos(300) = -0.0220966...
    CHECK(strictRange("-0.0220967", cos(PreciseRange(300)), "-0.0220966"), isTrue());

    // sin(-1000) = -0.82687954...
    CHECK(strictRange("-0.82687955", sin(PreciseRange(-1000)), "-0.82687954"), isTrue());
    // cos(-1000) = 0.56237907...
    CHECK(strictRange("0.56237907", cos(PreciseRange(-1000)), "0.56237908"), isTrue());
    // TODO: make sin/cos more efficient (and avoid overflowing the stack) for large values of x
}

TEST(PreciseRange, SinhCosh) {
    CHECK(sinh(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("0"));
    CHECK(cosh(PreciseRange(0)).toStringWithUncertaintyLog2AtMost(std::nullopt), equals("1"));
    CHECK(eq(sinh(PreciseRange("0.1") - PreciseRange("0.1")), PreciseRange(0)), isTrue());
    CHECK(eq(cosh(PreciseRange("0.1") - PreciseRange("0.1")), PreciseRange(1)), isTrue());

    // sinh(0.3) = 0.3045202...
    CHECK(strictRange("0.3045202", sinh(PreciseRange("0.3")), "0.3045203"), isTrue());
    // cosh(0.3) = 1.0453385...
    CHECK(strictRange("1.0453385", cosh(PreciseRange("0.3")), "1.0453386"), isTrue());

    // sinh(2.3) = 4.9369618...
    CHECK(strictRange("4.9369618", sinh(PreciseRange("2.3")), "4.9369619"), isTrue());
    // cosh(2.3) = 5.0372206...
    CHECK(strictRange("5.0372206", cosh(PreciseRange("2.3")), "5.0372207"), isTrue());
}

TEST(PreciseRange, SinhCoshLarge) {
    // sinh(-200) = -3.61298688...e+86
    CHECK(strictRange("-3.61298689", sinh(PreciseRange(-200)) / (PreciseRange(10) ^ 86), "-3.61298688"), isTrue());
    // cosh(-200) = 3.61298688...e+86
    CHECK(strictRange("3.61298688", cosh(PreciseRange(-200)) / (PreciseRange(10) ^ 86), "3.61298689"), isTrue());

    // sinh(-2000) = -1.94059009...e868
    CHECK(strictRange("-1.94059010", sinh(PreciseRange(-2000)) / (PreciseRange(10) ^ 868), "-1.94059009"), isTrue());
    // cosh(-2000) = 1.94059009...e868
    CHECK(strictRange("1.94059009", cosh(PreciseRange(-2000)) / (PreciseRange(10) ^ 868), "1.94059010"), isTrue());

    // TODO: see why sinh/cosh of 10000 are slow
    // sinh(10000) = 4.40340911...e4342
    //CHECK(strictRange("4.40340911", sinh(PreciseRange(10000)) / (PreciseRange(10) ^ 4342), "4.40340912"), isTrue());
    // cosh(10000) = 4.40340911...e4342
    //CHECK(strictRange("4.40340911", cosh(PreciseRange(10000)) / (PreciseRange(10) ^ 4342), "4.40340912"), isTrue());
}
