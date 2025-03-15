#include "PreciseRange.hpp"

#include "BigInt.hpp"
#include "DigitParser.hpp"
#include "ExitUtils.hpp"
#include "UniqueOwnedReferenceForCPP.hpp"
#include "Variant.hpp"

#include <cstdint>
#include <optional>

template<class... T>
[[noreturn]] void unimpl(T&&...) {
    throw_exception<std::logic_error>("unimplemented");
}

enum class Sign : int { Negative = -1, Zero = 0, Positive = 1 };

struct IntOrNegInf {
  public:
    // nullopt = -∞
    std::optional<std::int64_t> fValue;

    IntOrNegInf(std::int64_t x)
	: fValue(x) {}

    IntOrNegInf(std::optional<std::int64_t> x)
	: fValue(x) {}

  public:
    std::int64_t operator*() const {
	if (!fValue) {
	    throw_exception<std::logic_error>("Value was -∞");
	}
	return *fValue;
    }
    
  public:
    friend bool operator<(IntOrNegInf const& x, IntOrNegInf const& y) {
	if (!y.fValue) {
	    return false;
	}
	if (!x.fValue) {
	    return true;
	}
	return *x.fValue < *y.fValue;
    }
    friend bool operator>(IntOrNegInf const& x, IntOrNegInf const& y) {
	return y < x;
    }
    friend bool operator<=(IntOrNegInf const& x, IntOrNegInf const& y) {
	return !(y < x);
    }
    friend bool operator>=(IntOrNegInf const& x, IntOrNegInf const& y) {
	return !(x < y);
    }
    friend bool operator==(IntOrNegInf const& x, IntOrNegInf const& y) {
	return x.fValue == y.fValue;
    }
    friend bool operator!=(IntOrNegInf const& x, IntOrNegInf const& y) {
	return !(x == y);
    }

    std::string toString() const {
	if (!fValue) {
	    return "-∞";
	} else {
	    return std::to_string(*fValue);
	}
    }
};

struct BinaryShiftedInt {
  private:
    BigInt fIntValue;   // 5
    std::int64_t fExp2; // -2
                        // actual value = 1.25

  public:
    BinaryShiftedInt(BigInt intValue, std::int64_t exp2 = 0)
	: fIntValue(std::move(intValue))
	, fExp2(exp2) {
	if (fIntValue.inf()) {
	    throw_exception<std::domain_error>("Cannot construct infinite value: " + to_string(fIntValue));
	}
    }

    friend void swap(BinaryShiftedInt& x, BinaryShiftedInt& y) {
	using std::swap;
	swap(x.fIntValue, y.fIntValue);
	swap(x.fExp2, y.fExp2);
    }
    
    void negateMe() {
	fIntValue.negateMe();
    }

    explicit operator bool() const {
	return static_cast<bool>(fIntValue);
    }

    Sign sign() const {
	if (!fIntValue) {
	    return Sign::Zero;
	}
	if (fIntValue.neg()) {
	    return Sign::Negative;
	} else {
	    return Sign::Positive;
	}
    }

    std::int64_t rawExp2() const {
	return fExp2;
    }

    void shiftToHaveExponentLeq(std::int64_t maxExp) {
	if (fExp2 > maxExp) {
	    fIntValue <<= fExp2 - maxExp;
	    fExp2 = maxExp;
	}
    }

    friend IntOrNegInf floorLog2Abs(BinaryShiftedInt const& x) {
	if (!x.fIntValue) {
	    return IntOrNegInf{std::nullopt};
	}
	return IntOrNegInf{static_cast<std::int64_t>(x.fIntValue.log2()) + x.fExp2};
    }
    
    friend IntOrNegInf ceilLog2Abs(BinaryShiftedInt const& x) {
	if (!x.fIntValue) {
	    return IntOrNegInf{std::nullopt};
	}
	return IntOrNegInf{static_cast<std::int64_t>(x.fIntValue.log2()) + (x.fIntValue.isPowerOf2() ? 0 : 1) + x.fExp2};
    }

    struct ComparisonResult {
	struct Different {
	    bool fFirstArgIsLess;
	    std::int64_t fMaxDistLog2;
	};
	std::optional<Different> fResultIfDifferent;

	static constexpr std::optional<Different> Same = std::nullopt;
    };
    
    friend ComparisonResult comp(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	bool xZero = !x.fIntValue;
	bool yZero = !y.fIntValue;
	auto xNeg = x.fIntValue.neg();
	auto yNeg = y.fIntValue.neg();
	if (xZero && yZero) {
	    return {ComparisonResult::Same};
	}
	if (xZero) {
	    auto xLess = !yNeg;
	    return {ComparisonResult::Different{xLess, *ceilLog2Abs(y)}};
	}
	if (yZero) {
	    auto xLess = xNeg;
	    return {ComparisonResult::Different{xLess, *ceilLog2Abs(x)}};
	}
	auto highestBitX = static_cast<std::int64_t>(x.fIntValue.log2()) + x.fExp2;
	auto highestBitY = static_cast<std::int64_t>(y.fIntValue.log2()) + y.fExp2;
	auto lowestBitX = x.fExp2;
	auto lowestBitY = y.fExp2;
	auto highestBit = std::max(highestBitX, highestBitY);
	auto lowestBit = std::min(lowestBitX, lowestBitY);
	if (xNeg != yNeg) {
	    // Why +2? 111 - -11 > 1000 so we conservatively say 10000 via +2 from highest bit in either number.
	    auto ceilLog2Diff = highestBit + 2;
	    auto xLess = xNeg;
	    return {ComparisonResult::Different{xLess, ceilLog2Diff}};
	}
	ComparisonResult result{ComparisonResult::Same};
	for (auto bit = highestBit; bit >= lowestBit; --bit) {
	    auto xBit = bit >= x.fExp2 ? x.fIntValue.getBit(static_cast<std::size_t>(bit - x.fExp2)) : false;
	    auto yBit = bit >= y.fExp2 ? y.fIntValue.getBit(static_cast<std::size_t>(bit - y.fExp2)) : false;
	    if (xBit != yBit) {
		auto xLess = xBit == xNeg;
		if (result.fResultIfDifferent) {
		    // We already have a result from an earlier bit (for example, 100).
		    // Now we're just determining the upper bound on the distance.
		    if (result.fResultIfDifferent->fFirstArgIsLess == xLess) {
			// 110 vs 001 -> dist ≤ 1000
			++result.fResultIfDifferent->fMaxDistLog2;
		    } else {
			// 101 vs 010 -> dist ≤ 100
		    }
		    return result;
		}
		// 1?? vs 0?? -> tentatively mark distance as 100
		result.fResultIfDifferent = ComparisonResult::Different{xLess, bit};
	    }
	}
	return result;
    }

    friend bool operator<(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	auto res = comp(x, y);
	if (!res.fResultIfDifferent) {
	    return false;
	}
	return res.fResultIfDifferent->fFirstArgIsLess;
    }
    friend bool operator>(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return y < x;
    }
    friend bool operator<=(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return !(y < x);
    }
    friend bool operator>=(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return !(x < y);
    }
    friend bool operator==(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	auto res = comp(x, y);
	return !res.fResultIfDifferent;
    }
    friend bool operator!=(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return !(x == y);
    }

    friend BinaryShiftedInt operator+(BinaryShiftedInt x, BinaryShiftedInt const& y) {
	auto minExp = std::min(x.fExp2, y.fExp2);
        x.fIntValue <<= x.fExp2 - minExp;
	x.fExp2 = minExp;
	x.fIntValue += y.fIntValue << (y.fExp2 - minExp);
	return x;
    }

    friend BinaryShiftedInt operator-(BinaryShiftedInt x, BinaryShiftedInt const& y) {
	auto minExp = std::min(x.fExp2, y.fExp2);
        x.fIntValue <<= x.fExp2 - minExp;
	x.fExp2 = minExp;
	x.fIntValue -= y.fIntValue << (y.fExp2 - minExp);
	return x;
    }

    friend BinaryShiftedInt operator*(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return BinaryShiftedInt{x.fIntValue * y.fIntValue, x.fExp2 + y.fExp2};
    }

    enum class Round { TowardsZero, AwayFromZero };
    template<Round round>
    BinaryShiftedInt divide(BinaryShiftedInt const& denominator) const {
	return BinaryShiftedInt{round == Round::AwayFromZero
	    ? divideRoundAwayFrom0(fIntValue, denominator.fIntValue)
	    : fIntValue / denominator.fIntValue,
	    fExp2 - denominator.fExp2};
    }

    friend BinaryShiftedInt operator<<(BinaryShiftedInt x, std::int64_t shift) {
	x.fExp2 += shift;
	return x;
    }

    friend BinaryShiftedInt operator>>(BinaryShiftedInt x, std::int64_t shift) {
	x.fExp2 -= shift;
	return x;
    }
    
    std::string toString() const {
	if (fExp2 >= 0) {
	    return static_cast<std::string>(fIntValue << fExp2);
	} else {
	    auto negExp = static_cast<std::size_t>(-fExp2);
	    auto result = static_cast<std::string>(fIntValue * (BigInt(5) ^ negExp));
	    if (result.size() <= negExp) {
		result = std::string(negExp + 1 - result.size(), '0') + result;
	    }
	    result.insert(result.length() - negExp, ".");
	    while (result.back() == '0') {
		result.pop_back();
	    }
	    if (result.back() == '.') {
		result.pop_back();
	    }
	    return result;
	}
    }
};

struct BinaryShiftedIntRange {
  private:
    BinaryShiftedInt fLow;  // 1.25
    BinaryShiftedInt fHigh; // 1.5
                            // actual value is somewhere in range [1.25, 1.5]
    IntOrNegInf fUncertaintyLog2; // -2 for [1.25, 1.5]. nullopt = no uncertainty.

  public:
    BinaryShiftedIntRange(BinaryShiftedInt upperOrLowerBound, BinaryShiftedInt otherBound)
	: fLow(std::move(upperOrLowerBound))
	, fHigh(std::move(otherBound))
	, fUncertaintyLog2{std::nullopt} {
        auto cmp = comp(fLow, fHigh);
	if (auto resultIfDifferent = cmp.fResultIfDifferent) {
	    if (!resultIfDifferent->fFirstArgIsLess) {
		swap(fLow, fHigh);
	    }
	    fUncertaintyLog2 = resultIfDifferent->fMaxDistLog2;
	} else {
	    // same value - leave uncertainty unset
	}
    }

    IntOrNegInf const& uncertaintyLog2() const {
	return fUncertaintyLog2;
    }

    void negateMe() {
	swap(fLow, fHigh);
	fLow.negateMe();
	fHigh.negateMe();
    }
    
    struct RelationToZero {
	Sign fSign;
	BinaryShiftedInt const& fCloserToZero;
	BinaryShiftedInt const& fFurtherFromZero;
    };

    std::optional<RelationToZero> signIfKnown() const {
	auto lowSign = fLow.sign();
	auto highSign = fHigh.sign();
	if (lowSign == highSign) {
	    auto sign = lowSign;
	    if (sign == Sign::Positive) {
		return RelationToZero{sign, fLow, fHigh};
	    } else {
		return RelationToZero{sign, fHigh, fLow};
	    }
	} else {
	    return std::nullopt;
	}
    }

    std::optional<RelationToZero> relationToZeroIncludingZeroInBothSides() const {
	bool notNeg = fLow.sign() != Sign::Negative;
	bool notPos = fHigh.sign() != Sign::Positive;
	if (notNeg && notPos) {
	    return RelationToZero{Sign::Zero, fLow, fHigh};
	}
	if (notNeg) {
	    return RelationToZero{Sign::Positive, fLow, fHigh};
	}
	if (notPos) {
	    return RelationToZero{Sign::Negative, fHigh, fLow};
	}
	// could be positive or negative
	return std::nullopt;
    }

    std::int64_t minExp() const {
	return std::min(fLow.rawExp2(), fHigh.rawExp2());
    }

    void shiftToHaveExponentsLeq(std::int64_t maxExp) {
	fLow.shiftToHaveExponentLeq(maxExp);
	fHigh.shiftToHaveExponentLeq(maxExp);
    }

    friend IntOrNegInf maxCeilLog2Abs(BinaryShiftedIntRange const& x) {
	return std::max(ceilLog2Abs(x.fLow), ceilLog2Abs(x.fHigh));
    }

    friend IntOrNegInf minFloorLog2Abs(BinaryShiftedIntRange const& x) {
	return std::min(floorLog2Abs(x.fLow), floorLog2Abs(x.fHigh));
    }

    friend BinaryShiftedIntRange operator+(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y) {
	// TODO: consider shortening result if we're storing many bits beyond our uncertainty
	//   (for this and other arithmetic operators)
	return {x.fLow + y.fLow, x.fHigh + y.fHigh};
    }

    friend BinaryShiftedIntRange operator-(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y) {
	return {x.fLow - y.fHigh, x.fHigh - y.fLow};
    }
    struct NecessaryArgUncertainty {
	std::int64_t fMaxLhsUncertaintyLog2;
	std::int64_t fMaxRhsUncertaintyLog2;
    };
  private:
    friend Variant<BinaryShiftedIntRange, NecessaryArgUncertainty> productWithPossibleDesiredEstimate(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y, std::optional<std::int64_t> maxUncertaintyLog2ifNecessary) {
	// total uncertainty ≤ lhs_uncertainty * max_abs_rhs + rhs_uncertainty * max_abs_lhs
	// [a,b] * [c,d] -> [min(ac,ad,bc,bd), max(ac,ad,bc,bd)]
	// uncertainty of result
	// = max(ac,ad,bc,bd) - min(ac,ad,bc,bd)
	// ≤ max(|bc-ac|,|bd-ad|) + max(|ad-ac|,|bd-bc|)
	// = (b-a) * max(|c|,|d|) + (d-c) * max(|a|,|b|)
	// = lhs_uncertainty * max_abs_rhs + rhs_uncertainty * max_abs_lhs
	// If we request a refined range for an input, its max_abs cannot increase.
	// Thus we can use the current max_abs to determine max uncertainties for the args that will guarantee a max uncertainty for the result.
	auto xRelZero = x.relationToZeroIncludingZeroInBothSides();
	auto yRelZero = y.relationToZeroIncludingZeroInBothSides();
	if (xRelZero && yRelZero) {
	    auto closerToZero = xRelZero->fCloserToZero * yRelZero->fCloserToZero;
	    auto furtherFromZero = xRelZero->fFurtherFromZero * yRelZero->fFurtherFromZero;
	    BinaryShiftedIntRange result{closerToZero, furtherFromZero};
	    if (!maxUncertaintyLog2ifNecessary || result.uncertaintyLog2() <= *maxUncertaintyLog2ifNecessary) {
		return std::move(result);
	    } else {
		// uncertainty
		//   = |xFar*yFar-xNear*yNear|
		//   = |xFar*yFar-xNear*yFar+xNear*yFar-xNear*yNear|
		//   ≤ |xFar-xNear|*|yFar| + |yFar-yNear|*|xNear|
		//   = xUncertainty*|yFar| + yUncertainty*|xNear|
		// so we want to bound xUncertainty and yUncertainty so that, after getting new refined ranges for x and y, the above uncertainty obeys the desired uncertainty.
		// We know that |yFar_new| ≤ |yFar_old|, and |xNear_new| ≤ |xFar_old|, so we can pick:
		//   xUncertainty = 2^(maxUncertaintyLog2-1-ceil(log2(|yFar|)))
		//   yUncertainty = 2^(maxUncertaintyLog2-1-ceil(log2(|xFar|)))
		// bounding the overall uncertainty by:
		// uncertainty
		//   = xUncertainty*|yFar_new| + yUncertainty*|xNear_new|
		//   ≤ xUncertainty*|yFar_old| + yUncertainty*|xFar_old|
		//   ≤ 2^(maxUncertaintyLog2-1-ceil(log2(|yFar|)))*|yFar| + 2^(maxUncertaintyLog2-1-ceil(log2(|xFar|)))*|xFar|
		//   ≤ 2^(maxUncertaintyLog2-1) + 2^(maxUncertaintyLog2-1)
		//   = 2^maxUncertaintyLog2
		return NecessaryArgUncertainty{
		    *maxUncertaintyLog2ifNecessary - 1 - *ceilLog2Abs(yRelZero->fFurtherFromZero),
		    *maxUncertaintyLog2ifNecessary - 1 - *ceilLog2Abs(xRelZero->fFurtherFromZero)
		};
	    }
	} else if (xRelZero) {
	    BinaryShiftedIntRange result{xRelZero->fFurtherFromZero * y.fLow, xRelZero->fFurtherFromZero * y.fHigh};
	    if (!maxUncertaintyLog2ifNecessary || result.uncertaintyLog2() <= *maxUncertaintyLog2ifNecessary) {
		return std::move(result);
	    } else {
		// y is on both sides of zero - multiply both of its bounds by x's further bound
		// uncertainty when y is on both sides of 0
		//   = yUncertainty*|xFar|
		// but we need to succeed if y becomes on one side of 0.
		// By the logic above, the following choices work for that case:
		//   xUncertainty = 2^(maxUncertaintyLog2-1-ceil(log2(|yFar|)))
		//   yUncertainty = 2^(maxUncertaintyLog2-1-ceil(log2(|xFar|)))
		// and we see that, if y remains on both sides of 0 after refining:
		// uncertainty
		//   ≤ 2^(maxUncertaintyLog2-1-ceil(log2(|xFar|)))*|xFar|
		//   ≤ 2^(maxUncertaintyLog2-1)
		//   < 2^maxUncertaintyLog2
		return NecessaryArgUncertainty{
		    *maxUncertaintyLog2ifNecessary - 1 - *maxCeilLog2Abs(y),
		    *maxUncertaintyLog2ifNecessary - 1 - *ceilLog2Abs(xRelZero->fFurtherFromZero)
		};
	    }
	} else if (yRelZero) {
	    // x is on both sides of zero - multiply both of its bounds by y's further bound
	    BinaryShiftedIntRange result{yRelZero->fFurtherFromZero * x.fLow, yRelZero->fFurtherFromZero * x.fHigh};
	    if (!maxUncertaintyLog2ifNecessary || result.uncertaintyLog2() <= *maxUncertaintyLog2ifNecessary) {
		return std::move(result);
	    } else {
		// symmetric to xRelZero case above
		return NecessaryArgUncertainty{
		    *maxUncertaintyLog2ifNecessary - 1 - *ceilLog2Abs(yRelZero->fFurtherFromZero),
		    *maxUncertaintyLog2ifNecessary - 1 - *maxCeilLog2Abs(x)
		};
	    }
	} else {
	    // x and y are both on both sides of 0.
	    // For new lower bound, consider low*high and high*low (negative)
	    // For new upper bound, consider low*low and high*high (positive)
	    BinaryShiftedIntRange result{
		std::min(x.fLow * y.fHigh, x.fHigh * y.fLow),
		std::max(x.fLow * y.fLow, x.fHigh * y.fHigh)
	    };
	    if (!maxUncertaintyLog2ifNecessary || result.uncertaintyLog2() <= *maxUncertaintyLog2ifNecessary) {
		return std::move(result);
	    } else {
		// Similar uncertainty logic as above
		return NecessaryArgUncertainty{
		    *maxUncertaintyLog2ifNecessary - 1 - *maxCeilLog2Abs(y),
		    *maxUncertaintyLog2ifNecessary - 1 - *maxCeilLog2Abs(x)
		};
	    }
	}
    }
  public:
    friend BinaryShiftedIntRange operator*(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y) {
	auto result = productWithPossibleDesiredEstimate(x, y, std::nullopt);
	auto successfulResult = std::get_if<BinaryShiftedIntRange>(&result);
	if (!successfulResult) {
	    throw_exception<std::logic_error>("Multiplication without necessary precision should always return a successful result");
	}
	return std::move(*successfulResult);
    }
    friend Variant<BinaryShiftedIntRange, NecessaryArgUncertainty> productWithDesiredEstimate(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y, std::int64_t maxUncertaintyLog2) {
	return productWithPossibleDesiredEstimate(x, y, maxUncertaintyLog2);
    }
    // throws if y is/contains 0
    friend BinaryShiftedIntRange operator/(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y) {
	auto ySignIfKnown = y.signIfKnown();
	if (!ySignIfKnown) {
	    throw_exception<std::domain_error>("Division denominator includes 0: " + y.toString());
	}
	if (ySignIfKnown->fSign == Sign::Zero) {
	    throw_exception<std::domain_error>("Division denominator = 0: " + y.toString());
	}
	if (auto xRelZero = x.relationToZeroIncludingZeroInBothSides()) {
	    // both bounds of x have the same sign
	    return {
		xRelZero->fFurtherFromZero.divide<BinaryShiftedInt::Round::AwayFromZero>(ySignIfKnown->fCloserToZero),
		xRelZero->fCloserToZero.divide<BinaryShiftedInt::Round::TowardsZero>(ySignIfKnown->fFurtherFromZero)
	    };
	} else {
	    // x is on both sides of 0
	    return {
		x.fLow.divide<BinaryShiftedInt::Round::AwayFromZero>(ySignIfKnown->fCloserToZero),
		x.fHigh.divide<BinaryShiftedInt::Round::AwayFromZero>(ySignIfKnown->fCloserToZero)
	    };
	}
    }

    friend BinaryShiftedIntRange distanceToNearestInteger(BinaryShiftedIntRange const& x) {
	if (x.uncertaintyLog2() >= 1) {
	    // Since uncertainty is in [1, 2), this range extends for at least 1, thus includes low (0) and high (0.5) values for the distance to an integer
	    BinaryShiftedInt half(1, -1);
	    return {BinaryShiftedInt(0), std::move(half)};
	}
	if (auto xRelZero = x.relationToZeroIncludingZeroInBothSides()) {
	    // both bounds of x have the same sign
	    auto low = std::move(x.fLow);
	    auto high = std::move(x.fHigh);
	    auto res = comp(low, high);
	    if (!res.fResultIfDifferent) {
		// exact value
	    }
	    // TODO
	    unimpl(res, low, high);
	} else {
	    // x is on both sides of 0
	    auto v1 = std::move(x.fLow);
	    auto v2 = std::move(x.fHigh);
	    v1.negateMe(); // x.fLow < 0
	    BinaryShiftedInt half(1, -1);
	    return {BinaryShiftedInt(0), std::move(std::min(std::max(v1, v2), half))};
	}
    }

    std::string toString() const {
	auto diffToMidpoint = (fHigh - fLow) >> 1;
	if (!diffToMidpoint) {
	    return fLow.toString();
	}
	auto middle = fLow + diffToMidpoint;
	return "[" + middle.toString() + " ± " + diffToMidpoint.toString() + "]";
    }
};

enum class OperatorPriority : std::uint8_t {
    AdditionSubtraction = 0,    // inside any other expression, requires being shown in ()
    MultiplicationDivision = 1, // inside another * or / or unary fcn, requires being shown in ()
    ExactValue = 2,             // inside a unary fcn, requires being shown in ()
    UnaryOperator = 3,          // force any sub-expression to be shown in ()
};

struct AdjustablePrecisionRange {
  public:
    virtual ~AdjustablePrecisionRange() = default;

  private:
    virtual OperatorPriority operatorPriority() const = 0;
    virtual std::string toStringExact() const = 0;
    virtual BinaryShiftedIntRange calculateEstimate() = 0;
    virtual BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) = 0;

  public:
    std::string toStringExact(AdjustablePrecisionRange const* parent) const {
	if (parent && parent->operatorPriority() >= operatorPriority()) {
	    return "(" + toStringExact() + ")";
	} else {
	    return toStringExact();
	}
    }
    std::string toStringWithUncertaintyLog2AtMost(std::optional<std::int64_t> maxUncertaintyLog2) {
	if (maxUncertaintyLog2) {
	    return withUncertaintyLog2AtMost(*maxUncertaintyLog2).toString();
	}
	return estimate().toString();
    }
    BinaryShiftedIntRange const& estimate() {
	if (!fCachedValue) {
	    fCachedValue = calculateEstimate();
	}
	return *fCachedValue;
    }
    BinaryShiftedIntRange const& withUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) {
	bool cachedValueOK = fCachedValue && fCachedValue->uncertaintyLog2() <= maxUncertaintyLog2;
	if (!cachedValueOK) {
	    fCachedValue = calculateUncertaintyLog2AtMost(maxUncertaintyLog2);
	}
	return *fCachedValue;
    }
    struct RefinedWithSign {
	BinaryShiftedIntRange const& fRange;
	Sign fSign;
    };
    RefinedWithSign refineUntilSignIsKnown() {
	auto const& est = estimate();
	if (auto sign = est.signIfKnown()) {
	    return {est, sign->fSign};
	}
	// Dereference is safe: if there was no uncertainty, the sign would be known.
	std::int64_t const estimateUncertaintyLog2 = *est.uncertaintyLog2().fValue;
	auto uncertaintyLog2 = estimateUncertaintyLog2;
	for (std::int64_t extraPrecision = 1; extraPrecision <= 1024; extraPrecision *= 2) {
	    uncertaintyLog2 = estimateUncertaintyLog2 - extraPrecision;
	    BinaryShiftedIntRange const& result = withUncertaintyLog2AtMost(uncertaintyLog2);
	    if (auto sign = result.signIfKnown()) {
		return {result, sign->fSign};
	    }
	}
	throw_exception<std::logic_error>("With uncertainty ≤ 2^(" + std::to_string(uncertaintyLog2) + "), could not determine " + toStringExact(nullptr));
    }

    BinaryShiftedIntRange const& refineUntilSignIsKnownAndRangeInFactorOf2() {
	auto const& range = refineUntilSignIsKnown().fRange;
	auto maximumLog2Uncertainty = *minFloorLog2Abs(range);
	if (range.uncertaintyLog2() <= maximumLog2Uncertainty) {
	    return range;
	}
	return withUncertaintyLog2AtMost(maximumLog2Uncertainty);
    }

    template<class... T>
    [[noreturn]] void unimpl(T&&...) const {
	throw_exception<std::logic_error>("unimplemented: " + toStringExact());
    }

  private:
    std::optional<BinaryShiftedIntRange> fCachedValue;
};

struct ExactValue : public AdjustablePrecisionRange {
  public:
    ExactValue(BinaryShiftedInt value)
	: fBinaryShiftedInt(std::move(value)) {}

  private:
    BinaryShiftedIntRange exactRange() const {
	return {fBinaryShiftedInt, fBinaryShiftedInt};
    }
    OperatorPriority operatorPriority() const final {
	return OperatorPriority::ExactValue;
    }
    std::string toStringExact() const final {
	return fBinaryShiftedInt.toString();
    }
    BinaryShiftedIntRange calculateEstimate() final {
	return exactRange();
    }
    BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t /* maxUncertaintyLog2 */) final {
	return exactRange();
    }

  private:
    BinaryShiftedInt fBinaryShiftedInt;
};

using SharedPreciseRange = std::shared_ptr<AdjustablePrecisionRange>;

struct PreciseRange::Impl : public SharedPreciseRange {
    using SharedPreciseRange::SharedPreciseRange;
    
    static Impl fromNumeric(std::string_view numericLiteral) {
	B negative = false;
	if (!numericLiteral.empty() && numericLiteral[0] == '-') {
	    negative = true;
	    numericLiteral.remove_prefix(1);
	}
	DigitParser digitParser = DecimalParser;
	if (numericLiteral.size() >= 2 && numericLiteral[0] == '0') {
	    if (auto parserFromPrefix = DigitParser::fromPrefixChar(numericLiteral[1])) {
		digitParser = *parserFromPrefix;
		numericLiteral.remove_prefix(2);
	    }
	}
	BigInt numerator;
	B encounteredDecimalPoint = false;
	B anyDigits = false;
        UI denomExp = 0;
	for (C c : numericLiteral) {
	    if (auto nextDigit = digitParser.fCharToDigit(c)) {
		anyDigits = true;
		numerator *= digitParser.fMultPerDigit;
		numerator += *nextDigit;
		if (encounteredDecimalPoint) {
		    ++denomExp;
		}
	    } else if (!encounteredDecimalPoint && c == '.') {
		encounteredDecimalPoint = true;
	    } else {
		throw_exception<std::invalid_argument>(std::string("Unexpected character when parsing PreciseRange: '") + c + "' (" + std::to_string(static_cast<SI>(c)) + ")");
	    }
	}
	if (!anyDigits) {
	    throw_exception<std::invalid_argument>("PreciseRange cannot be constructed from an empty string (after possible negation and base prefix and decimal point)");
	}
	if (negative) {
	    numerator.negateMe();
	}
	if (denomExp == 0) {
	    return std::make_shared<ExactValue>(std::move(numerator));
	} else if (digitParser.fExactLog2) {
	    auto exp2 = -static_cast<std::int64_t>(*digitParser.fExactLog2 * denomExp);
	    return std::make_shared<ExactValue>(BinaryShiftedInt(std::move(numerator), exp2));
	} else {
	    BigInt denominator = BigInt(digitParser.fMultPerDigit) ^ denomExp;
	    PreciseRange fraction =
		PreciseRange(Impl(std::make_shared<ExactValue>(std::move(numerator)))) /
		PreciseRange(Impl(std::make_shared<ExactValue>(std::move(denominator))));
	    return fraction.impl();
	}
    }
};

struct PreciseUnaryOp : public AdjustablePrecisionRange {
    SharedPreciseRange fArg;

    // Necessary for derived classes to be able to "using PreciseUnaryOp::PreciseUnaryOp;"
    PreciseUnaryOp(SharedPreciseRange arg)
	: fArg(arg) {}
    
    virtual std::string op() const = 0;
    OperatorPriority operatorPriority() const final {
	return OperatorPriority::UnaryOperator;
    }
    std::string toStringExact() const final {
	return op() + fArg->toStringExact(this);
    }
};

struct PreciseBinaryOp : public AdjustablePrecisionRange {
    SharedPreciseRange fLhs;
    SharedPreciseRange fRhs;

    // Necessary for derived classes to be able to "using PreciseBinaryOp::PreciseBinaryOp;"
    PreciseBinaryOp(SharedPreciseRange lhs, SharedPreciseRange rhs)
	: fLhs(lhs)
	, fRhs(rhs) {}
    
    virtual std::string op() const = 0;
    std::string toStringExact() const final {
	return fLhs->toStringExact(this) + op() + fRhs->toStringExact(this);
    }
};

PreciseRange::PreciseRange(std::int64_t value)
    : fImpl(std::make_shared<ExactValue>(BigInt(value))) {}

PreciseRange::PreciseRange(std::string_view numericLiteral)
    : fImpl(Impl::fromNumeric(numericLiteral)) {}

PreciseRange::PreciseRange(PreciseRange const& other)
    : fImpl(other.impl()) {}

PreciseRange& PreciseRange::operator=(PreciseRange const& other) {
    static_cast<Impl&>(fImpl) = other.impl();
    return *this;
}

PreciseRange::~PreciseRange() = default;

PreciseRange PreciseRange::pi() {
    struct Pi : public AdjustablePrecisionRange {
      private:
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::ExactValue;
	}
	std::string toStringExact() const final {
	    return "π";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    // https://en.wikipedia.org/wiki/Gauss%E2%80%93Legendre_algorithm
	    // https://en.wikipedia.org/wiki/Pi#Computer_era_and_iterative_algorithms
	    unimpl(maxUncertaintyLog2);
	}
    };
    return Impl{std::make_shared<Pi>()};
}

PreciseRange PreciseRange::operator-() const {
    struct Negative : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "-";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    auto result = fArg->estimate();
	    result.negateMe();
	    return result;
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto result = fArg->withUncertaintyLog2AtMost(maxUncertaintyLog2);
	    result.negateMe();
	    return result;
	}
    };
    return Impl{std::make_shared<Negative>(impl())};
}

PreciseRange sqrt(PreciseRange const& x) {
    struct Sqrt : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "√";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // √(n) ≈ (n + nearest_square) / (2  * √nearest_square)
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    // https://en.wikipedia.org/wiki/Integer_square_root
	    // https://stackoverflow.com/questions/10725522/arbitrary-precision-of-square-roots
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Sqrt>(x.impl())};
}

PreciseRange distanceToNearestInteger(PreciseRange const& x) {
    struct DistanceToNearestInteger : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "distanceToNearestInteger";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return distanceToNearestInteger(fArg->estimate());
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return distanceToNearestInteger(fArg->withUncertaintyLog2AtMost(maxUncertaintyLog2));
	}
    };
    return PreciseRange::Impl{std::make_shared<DistanceToNearestInteger>(x.impl())};
}

PreciseRange exp(PreciseRange const& x) {
    struct Exp : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "e^";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Exp>(x.impl())};
}

PreciseRange sin(PreciseRange const& x) {
    struct Sin : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "sin";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Sin>(x.impl())};
}

PreciseRange cos(PreciseRange const& x) {
    struct Cos : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "cos";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Cos>(x.impl())};
}

PreciseRange sinh(PreciseRange const& x) {
    struct Sinh : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "sinh";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Sinh>(x.impl())};
}

PreciseRange cosh(PreciseRange const& x) {
    struct Cosh : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "cosh";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    // TODO
	    unimpl(fArg);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // TODO
	    unimpl(fArg, maxUncertaintyLog2);
	}
    };
    return PreciseRange::Impl{std::make_shared<Cosh>(x.impl())};
}

PreciseRange operator+(PreciseRange const& x, PreciseRange const& y) {
    struct Sum : public PreciseBinaryOp {
	using PreciseBinaryOp::PreciseBinaryOp;
	std::string op() const final {
	    return "+";
	}
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::AdditionSubtraction;
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return fLhs->estimate() + fRhs->estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return fLhs->withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1) + fRhs->withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1);
	}
    };
    return PreciseRange::Impl{std::make_shared<Sum>(x.impl(), y.impl())};
}

PreciseRange operator-(PreciseRange const& x, PreciseRange const& y) {
    struct Difference : public PreciseBinaryOp {
	using PreciseBinaryOp::PreciseBinaryOp;
	std::string op() const final {
	    return "-";
	}
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::AdditionSubtraction;
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return fLhs->estimate() - fRhs->estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return fLhs->withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1) - fRhs->withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1);
	}
    };
    return PreciseRange::Impl{std::make_shared<Difference>(x.impl(), y.impl())};
}

PreciseRange operator*(PreciseRange const& x, PreciseRange const& y) {
    struct Product : public PreciseBinaryOp {
	using PreciseBinaryOp::PreciseBinaryOp;
	std::string op() const final {
	    return "*";
	}
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::MultiplicationDivision;
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return fLhs->estimate() * fRhs->estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto productOrNecessaryArgUncertainty = productWithDesiredEstimate(fLhs->estimate(), fRhs->estimate(), maxUncertaintyLog2);
	    auto [asProduct, asNecessaryArgUncertainty] = productOrNecessaryArgUncertainty.split();
	    if (asProduct) {
		return std::move(*asProduct);
	    } else {
		auto lhsMoreCertain = fLhs->withUncertaintyLog2AtMost(asNecessaryArgUncertainty->fMaxLhsUncertaintyLog2);
		auto rhsMoreCertain = fRhs->withUncertaintyLog2AtMost(asNecessaryArgUncertainty->fMaxRhsUncertaintyLog2);
		return lhsMoreCertain * rhsMoreCertain;
	    }
	}
    };
    return PreciseRange::Impl{std::make_shared<Product>(x.impl(), y.impl())};
}

PreciseRange operator/(PreciseRange const& x, PreciseRange const& y) {
    struct Quotient : public PreciseBinaryOp {
	using PreciseBinaryOp::PreciseBinaryOp;
	std::string op() const final {
	    return "/";
	}
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::MultiplicationDivision;
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return fLhs->estimate() / fRhs->refineUntilSignIsKnown().fRange;
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // [a,b] / [c,d] where C = |c| ≤ |d| = D, A = |a|, B = |b|
	    // a < 0 < b -> [⌊a/C⌋, ⌈b/C⌉] with uncertainty (b-a)/C + roundingUncertainty(a,C) + roundingUncertainty(b,C)
	    // 0 ≤ a ≤ b -> [⌊a/D⌋, ⌈b/C⌉] with uncertainty b/C-a/D + roundingUncertainty(a,D) + roundingUncertainty(b,C)
	    // roundingUncertainty(x,y) ≤ 2^(x.fExp2-y.fExp2), where we can shift x or y to adjust their fExp2.
	    // Notably, we can decrease .fExp2 while shifting the BigInt left (vs shifting the BigInt right could lose precision).
	    // Thus in order to decrease roundingUncertainty(x,y), we shift x.
	    // This makes intuitive sense - increase the magnitude of the numerator, with a more negative exponent.
	    // For example, when performing 6/7, and y stored as 7 * 2^0, we might shift x from 3 * 2^1 to 12 * 2^-1:
	    // ⌊3/7⌋ * 2^1 = 0, ⌈3/7⌉ * 2^1 = 2
	    // ⌊12/7⌋ * 2^-1 = 1/2, ⌈12/7⌉ * 2^-1 = 1

	    // We may choose:
	    // 1. maximum uncertainty of numerator ≥ |b-a|
	    // 2. maximum uncertainty of denominator ≥ |d-c|, or refining to get |c| ≤ |d|/2
	    // 3. shifting of numerator values before performing division calculations
	    
	    // First we refine the denominator to guarantee C ≤ D ≤ 2C
	    auto denominator = fRhs->refineUntilSignIsKnownAndRangeInFactorOf2();
	    auto log2LowerBoundDenom = *minFloorLog2Abs(denominator); // L = ⌊log2(C)⌋, and 2^L ≤ C
	    // choose an uncertainty on a and b so (b-a)/C and b/C-a/D are both ≤ 2^(maxUncertaintyLog2-1)
	    // (b-a)/C ≤ 2^numeratorUncertaintyLog2/2^L = 2^(numeratorUncertaintyLog2-L) ≤ 2^(maxUncertaintyLog2-1)
	    // which is satisfied when numeratorUncertaintyLog2 ≤ L + maxUncertaintyLog2 - 1
	    // For b/C-a/D, we refine the numerator so that either:
	    // * b gets small enough that [0,b/C] has the desired uncertainty
	    // * a gets big enough that b/C-a/D has the desired uncertainty: (bD-aC)/(CD)
	    auto numerator = fLhs->estimate();
	    auto result = numerator / denominator;
	    if (result.uncertaintyLog2() <= maxUncertaintyLog2) {
		return result;
	    }
	    auto resultSign = result.signIfKnown();
	    if (!resultSign) {
		// We seek k so that, for Q,R,S,T satisfying A ≤ Q ≤ 0 ≤ R ≤ B, R-Q ≤ 2^k, C ≤ S ≤ T ≤ D,
		// R/T-Q/T ≤ 2^(maxUncertaintyLog2-1)
		// R/T-Q/T
		//  = (R-Q)/T
		//  ≤ 2^k/T
		//  ≤ 2^k/C
		//  ≤ 2^k/2^⌊log2(C)⌋
		//  = 2^(k-⌊log2(C)⌋)
		// We choose k = ⌊log2(C)⌋ + maxUncertaintyLog2 - 1
		auto k = log2LowerBoundDenom + maxUncertaintyLog2 - 1;
		numerator = fLhs->withUncertaintyLog2AtMost(k);
		if (!numerator.signIfKnown()) {
		    // Next we limit the uncertainty when performing rounding during division.
		    // Specifically, the uncertainty is at most 2^(numeratorExp-denominatorExp)
		    // We need this to be ≤ 2^(maxUncertaintyLog2-2) on each end, so the uncertainty from rounding both ends is ≤ 2^(maxUncertaintyLog2-1)
		    // Thus we require numeratorExp-denominatorExp ≤ maxUncertaintyLog2-2, so:
		    // numeratorExp ≤ maxUncertaintyLog2-2+denominatorExp
		    numerator.shiftToHaveExponentsLeq(maxUncertaintyLog2-2+denominator.minExp());
		    result = numerator / denominator;
		    if (result.uncertaintyLog2() <= maxUncertaintyLog2) {
			return result;
		    } else {
			throw_exception<std::logic_error>("Division for numerator without a sign failed to meet expected precision: " + numerator.toString() + " / " + denominator.toString() + " = " + result.toString() + " has uncertaintyLog2 = " + result.uncertaintyLog2().toString() + " >= " + std::to_string(maxUncertaintyLog2) + " (desired)");
		    }
		}
	    }
	    // numerator and denominator each lay on a specific side of 0.
	    // We seek k so that, for any Q,R,S,T satisfying A ≤ Q ≤ R ≤ B, R-Q ≤ 2^k, C ≤ S ≤ T ≤ D, T-S ≤ 2^k,
	    // R/S-Q/T ≤ 2^(maxUncertaintyLog2-1)
	    // R ≤ Q+2^k and S ≥ T-2^k
	    // R/S-Q/T
	    //  ≤ (Q+2^k)/(T-2^k)-Q/T
	    //  = (T*(Q+2^k)-Q*(T-2^k))/(T*(T-2^k))
	    //  = (T*2^k+Q*2^k)/(T*(T-2^k))
	    //  ≤ (1+Q/T)/(T-2^k)*2^k
	    //  ≤ (1+B/C)/(C-2^k)*2^k
	    //  ≤ (1+upperBoundOfAbsEstimate)/(C-2^k)*2^k
	    // We choose k ≤ log2(C)-1 so 2^k ≤ C/2, and C-2^k ≥ C/2, so...
	    //  ≤ (1+upperBoundOfAbsEstimate)/C*2^(k+1)
	    //  ≤ (1+2^log2AbsResultUpperBound)/(2^log2LowerBoundDenom)*2^(k+1)
	    //  ≤ 2^(max(log2AbsResultUpperBound,0)+1)/(2^log2LowerBoundDenom)*2^(k+1)
	    //  = 2^(max(log2AbsResultUpperBound,0)-log2LowerBoundDenom+k+2)
	    // We choose k ≤ log2LowerBoundDenom-3-max(log2AbsResultUpperBound,0)+maxUncertaintyLog2
	    //  ≤ 2^(maxUncertaintyLog2-1)
	    // Thus we can select k = min(log2LowerBoundDenom-1, log2LowerBoundDenom-3-max(log2AbsResultUpperBound,0)+maxUncertaintyLog2)
	    // k = log2LowerBoundDenom-1 + min(0, maxUncertaintyLog2-2-max(log2AbsResultUpperBound,0))
	    auto log2AbsResultUpperBound = *ceilLog2Abs(resultSign->fFurtherFromZero);
	    std::int64_t zero = 0;
	    auto k = log2LowerBoundDenom - 1 + std::min(zero, maxUncertaintyLog2 - 2 - std::max(zero, log2AbsResultUpperBound));
	    numerator = fLhs->withUncertaintyLog2AtMost(k);
	    denominator = fRhs->withUncertaintyLog2AtMost(k);
	    // Next we limit the uncertainty when performing rounding during division.
	    // Specifically, the uncertainty is at most 2^(numeratorExp-denominatorExp)
	    // We need this to be ≤ 2^(maxUncertaintyLog2-2) on each end, so the uncertainty from rounding both ends is ≤ 2^(maxUncertaintyLog2-1)
	    // Thus we require numeratorExp-denominatorExp ≤ maxUncertaintyLog2-2, so:
	    // numeratorExp ≤ maxUncertaintyLog2-2+denominatorExp
	    numerator.shiftToHaveExponentsLeq(maxUncertaintyLog2-2+denominator.minExp());
	    // The uncertainty from the rounding on both ends combined is at most 2^(maxUncertaintyLog2-1)
	    // The uncertainty in the resulting range is at most 2^(maxUncertaintyLog2-1)
	    // Thus the total uncertainty is at most 2^maxUncertaintyLog2
	    return numerator / denominator;
	}
    };
    return PreciseRange::Impl{std::make_shared<Quotient>(x.impl(), y.impl())};
}

bool operator<(PreciseRange const& x, PreciseRange const& y) {
    auto sub = x - y;
    return sub.impl()->refineUntilSignIsKnown().fSign == Sign::Negative;
}

bool operator>(PreciseRange const& x, PreciseRange const& y) {
    return y < x;
}

std::string PreciseRange::toStringExact() const {
    return impl()->toStringExact(nullptr);
}

std::string PreciseRange::toStringWithUncertaintyLog2AtMost(std::optional<std::int64_t> maxUncertaintyLog2) const {
    return impl()->toStringWithUncertaintyLog2AtMost(maxUncertaintyLog2);
}

PreciseRange::PreciseRange(Impl const& impl)
    : fImpl(impl) {}

PreciseRange::Impl const& PreciseRange::impl() const {
    return fImpl;
}
