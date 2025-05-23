#include "PreciseRange.hpp"

#include "BigInt.hpp"
#include "DigitParser.hpp"
#include "ExitUtils.hpp"
#include "Variant.hpp"

#include <cstdint>
#include <optional>

// An exception of this type indicates a logic error in this file. Namely, this code should be unhittable.
class PreciseRangeImplLogicError : private std::string, public std::exception {
  public:
    template<class... T>
    PreciseRangeImplLogicError(T&&... t)
	: std::string(std::forward<T>(t)...) {}

    char const* what() const noexcept override {
	return c_str();
    }
};

enum class Sign : int { Negative = -1, Zero = 0, Positive = 1 };
static PreciseRange::Cmp signToCmp(Sign sign) {
    switch (sign) {
      case Sign::Negative:
	return PreciseRange::Cmp::Less;
      case Sign::Zero:
	return PreciseRange::Cmp::ApproxEqual;
      case Sign::Positive:
	return PreciseRange::Cmp::Greater;
    }
}

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
	    throw_exception<PreciseRangeImplLogicError>("Value was -∞");
	}
	return *fValue;
    }
    
  public:
    friend IntOrNegInf operator+(IntOrNegInf x, std::int64_t other) {
	if (x.fValue) {
	    *x.fValue += other;
	}
	return x;
    }
    friend IntOrNegInf operator-(IntOrNegInf x, std::int64_t other) {
	if (x.fValue) {
	    *x.fValue -= other;
	}
	return x;
    }
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
	    throw_exception<PreciseRangeImplLogicError>("Cannot construct infinite value: " + to_string(fIntValue));
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

    void roundTowardsZero(std::int64_t granularityLog2) {
	if (fExp2 < granularityLog2) {
	    fIntValue >>= granularityLog2 - fExp2;
	    fExp2 = granularityLog2;
	}
    }

    // Note: this intentionally always adds/removes 1, even if there was no decmial to cause rounding.
    // For example, (6*2^-1).roundAwayFromZero(0) will shift to 3*2^0 and then increment to 4*2^0.
    // This is desirable for the callsites in BinaryShiftedInt that seek to truncate results.
    void roundAwayFromZero(std::int64_t granularityLog2) {
	if (fExp2 < granularityLog2) {
	    auto isNeg = fIntValue.neg(); // compute before shifting (which may change negative to 0)
	    fIntValue >>= granularityLog2 - fExp2;
	    if (isNeg) {
		--fIntValue;
	    } else {
		++fIntValue;
	    }
	    fExp2 = granularityLog2;
	}
    }

    BigInt const& upperBoundAbs() {
	roundAwayFromZero(0);
	shiftToHaveExponentLeq(0);
	if (sign() == Sign::Negative) {
	    negateMe();
	}
	return fIntValue;
    }

    void makeFloor() {
	if (fExp2 < 0) {
	    bool decrementAfterShifting = false;
	    if (fIntValue.neg()) {
		for (SZ i = 0, numBits = static_cast<SZ>(-fExp2); i < numBits; ++i) {
		    if (fIntValue.getBit(i)) {
			decrementAfterShifting = true;
			break;
		    }
		}
	    }
	    fIntValue <<= fExp2;
	    fExp2 = 0;
	    if (decrementAfterShifting) {
		--fIntValue;
	    }
	}
    }

    bool getBit(std::int64_t bit) const {
	auto bitInInt = bit - fExp2;
	if (bitInInt < 0) {
	    return false;
	}
	return fIntValue.getBit(static_cast<std::size_t>(bitInInt));
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
	return IntOrNegInf{static_cast<std::int64_t>(x.fIntValue.ceilLog2()) + x.fExp2};
    }

    struct ComparisonResult {
	struct Different {
	    bool fFirstArgIsLess;
	    std::int64_t fCeilLog2Dist;
	};
	std::optional<Different> fResultIfDifferent;

	static constexpr std::optional<Different> Same = std::nullopt;
    };

    template<B Truncate>
    using NonConstIf = Cond<Truncate, BinaryShiftedInt, BinaryShiftedInt const>;

    template<B Truncate>
    static ComparisonResult compMaybeTruncate(NonConstIf<Truncate>& x, NonConstIf<Truncate>& y) {
	bool xZero = !x.fIntValue;
	bool yZero = !y.fIntValue;
	auto xNeg = x.fIntValue.neg();
	auto yNeg = y.fIntValue.neg();
	if (xZero && yZero) {
	    return {ComparisonResult::Same};
	}
	if (xZero) {
	    auto xLess = !yNeg;
	    auto ceilLog2Dist = *ceilLog2Abs(y);
	    if constexpr (Truncate) {
		y = BinaryShiftedInt(yNeg ? -1 : 1, ceilLog2Dist);
	    }
	    return {ComparisonResult::Different{xLess, ceilLog2Dist}};
	}
	if (yZero) {
	    auto xLess = xNeg;
	    auto ceilLog2Dist = *ceilLog2Abs(x);
	    if constexpr (Truncate) {
		x = BinaryShiftedInt(xNeg ? -1 : 1, ceilLog2Dist);
	    }
	    return {ComparisonResult::Different{xLess, ceilLog2Dist}};
	}
	auto highestBitX = static_cast<std::int64_t>(x.fIntValue.log2()) + x.fExp2;
	auto highestBitY = static_cast<std::int64_t>(y.fIntValue.log2()) + y.fExp2;
	auto lowestBitX = x.fExp2;
	auto lowestBitY = y.fExp2;
	auto highestBit = std::max(highestBitX, highestBitY);
	auto lowestBit = std::min(lowestBitX, lowestBitY);
	if (xNeg != yNeg) {
	    auto xLess = xNeg;
	    // All the bits < bit x total to < bit x in each number, for a sum of < 2 * bit x.
	    // Possible outputs are highestBit+1 or highestBit+2.
	    // The output cannot be highestBit since this requires one number to be 0, which was handled above.
	    // If we sum to ≤ 2*2^highestBit, we return highestBit+1, otherwise return highestBit+2.
	    // We track the maximum amount of the current bit that we can accomodate and still return highestBit+1.
	    int maxAccomodate = 2;
	    for (auto bit = highestBit; bit >= lowestBit; --bit, maxAccomodate *= 2) {
		auto xBit = bit >= x.fExp2 ? x.fIntValue.getBit(static_cast<std::size_t>(bit - x.fExp2)) : false;
		auto yBit = bit >= y.fExp2 ? y.fIntValue.getBit(static_cast<std::size_t>(bit - y.fExp2)) : false;
		if (xBit) {
		    --maxAccomodate;
		}
		if (yBit) {
		    --maxAccomodate;
		}
		if (maxAccomodate < 0) {
		    // Difference > 2^(highestBit+1) -> ceilLog2Dist = 2^(highestBit+2)
		    // -1011... vs 0110...
		    if constexpr (Truncate) {
			// Round both away from zero to ±2^(highestBit+1) -> -10000 vs 10000
			// This has total difference highestBit+2 (the result).
			x = BinaryShiftedInt(xNeg ? -1 : 1, highestBit + 1);
			y = BinaryShiftedInt(yNeg ? -1 : 1, highestBit + 1);
		    }
		    return {ComparisonResult::Different{xLess, highestBit + 2}};
		}
		if (maxAccomodate >= 2) {
		    // All the remaining bits in both numbers can't sum to 2 of the current bit.
		    // -1010... vs 0100...
		    if constexpr (Truncate) {
			// Truncate by rounding both away from zero at this bit -> -1011 and 0101
			x.roundAwayFromZero(bit);
			y.roundAwayFromZero(bit);
		    }
		    return {ComparisonResult::Different{xLess, highestBit + 1}};
		}
	    }
	    // -101 vs 010 (maxAccomodate=2) OR -1010 vs 0110 (maxAccomodate=0)
	    // Could only round if maxAccomodate=2, but this is likely not worth it.
	    // Could trim trailing 0's, but this is likely not worth it.
	    return {ComparisonResult::Different{xLess, highestBit + 1}};
	}
	ComparisonResult result{ComparisonResult::Same};
	bool finalizedBiggestDiffBit = false;
	for (auto bit = highestBit; bit >= lowestBit; --bit) {
	    auto xBit = bit >= x.fExp2 ? x.fIntValue.getBit(static_cast<std::size_t>(bit - x.fExp2)) : false;
	    auto yBit = bit >= y.fExp2 ? y.fIntValue.getBit(static_cast<std::size_t>(bit - y.fExp2)) : false;
	    if (result.fResultIfDifferent) {
		if (xBit == yBit) {
		    // 10... vs 00... keep comparing bits to see if we use the bit from 10... or one larger
		    finalizedBiggestDiffBit = true; // 10... or 100...
		} else {
		    auto xLess = xBit == xNeg;
		    if (result.fResultIfDifferent->fFirstArgIsLess == xLess) {
			// 11... vs 00... -> since we got 2 bits contributing to the same side, increment and return
			// 111... vs 010... -> since we got 2 bits contributing to the same side, increment and return.
			++result.fResultIfDifferent->fCeilLog2Dist;
			if constexpr (Truncate) {
			    // 11... -> 100, 00... -> 00... (rounded down)
			    // 111 -> 1000, 010 -> 000
			    // round the number with the '1' bits away from 0 to the result-1 bit
			    // round the number with the '0' bits towards 0 to the result-1 bit
			    // Note that this may not be setting a bit in the number with the '1' bits,
			    //   but incrementing at that bit.
			    // For example, 1111 and 1001 -> dist = 11_ -> 1000, and
			    //   1111 -> (⌊1111/100⌋+1)*100 = 10000
			    //   1001 -> (⌊1001/100⌋)*100 = 1000
			    // Similarly, 1111 and 1010 -> dist = 101 -> 1000, and
			    //   1111 -> (⌊1111/100⌋+1)*100 = 10000
			    //   1010 -> (⌊1010/100⌋)*100 = 1000
			    // More interesting case: 0111100 vs 1000001
			    //   dist = 1000000 -> 100000 -> 10000 -> 1000 -> 100 then we process the final "1" and round the dist up to 1000.
			    //   This is similar to 101 vs 000, where we round 101 -> 1000, 000 -> 000, but the round up is from a 0 bit instead of a 1 bit, and the round down is from a 1 bit instead of a 0 bit.
			    // Round 0111100 down to 0111100
			    // Round 1000001   up to 1000100
			    // WOLOG let 0 < x < y.
			    // ⌊y/2^(resultBit-1)⌋ - ⌊x/2^(resultBit-1)⌋ = 1
			    // We round x down -> X = ⌊x/2^(resultBit-1)⌋*2^(resultBit-1)
			    // We round y   up -> Y = (⌊y/2^(resultBit-1)⌋+1)*2^(resultBit-1)
			    // Y-X
			    //   = (⌊y/2^(resultBit-1)⌋-⌊x/2^(resultBit-1)⌋+1)*2^(resultBit-1)
			    //   = (1+1)*2^(resultBit-1)
			    //   = 2^resultBit
			    // TODO: split this file into multiple in order to write tests for the pieces.
			    auto resultBit = result.fResultIfDifferent->fCeilLog2Dist;
			    if (xBit) {
				x.roundAwayFromZero(resultBit - 1);
				y.roundTowardsZero(resultBit - 1);
			    } else {
				x.roundTowardsZero(resultBit - 1);
				y.roundAwayFromZero(resultBit - 1);
			    }
			}
			return result; // 101... rounds up to 1000...
		    } else {
			if (finalizedBiggestDiffBit) {
			    // 110... vs 011... -> since we got 2 bits contributing to opposite sides, return existing bit
			    if constexpr (Truncate) {
				// Round 110... up to 111 and 011... down to 011 for exact difference 2^resultBit
				if (xBit) {
				    x.roundTowardsZero(bit);
				    y.roundAwayFromZero(bit);
				} else {
				    x.roundAwayFromZero(bit);
				    y.roundTowardsZero(bit);
				}
			    }
			    return result; // 100... - 001... -> rounds up to 100...
			} else {
			    // 10... vs 01... -> treat as 1... vs 0... and keep comparing bits (but first bit isn't finalized)
			    result.fResultIfDifferent->fCeilLog2Dist = bit; // 10... - 01... -> 01... and continue
			}
		    }
		}
	    } else {
		// no bit diff yet
		if (xBit != yBit) {
		    auto xLess = xBit == xNeg;
		    // 1... vs 0...
		    // We can't just stop at the first (or second) difference, since we must compare 0b101100001 vs 0b101011111 as only 10 apart.
		    result.fResultIfDifferent = ComparisonResult::Different{xLess, bit};
		}
	    }
	}
	if (result.fResultIfDifferent) {
	    // 110 vs 010
	    // Could trim trailing 0's, but this is likely not worth it.
	} else {
	    // 110 vs 110 - same value
	    // Could trim trailing 0's, but this is likely not worth it.
	}
	return result;
    }
    friend ComparisonResult comp(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	return compMaybeTruncate<false>(x, y);
    }
    friend ComparisonResult compTruncate(BinaryShiftedInt& x, BinaryShiftedInt& y) {
	return compMaybeTruncate<true>(x, y);
    }

    friend bool operator<(BinaryShiftedInt const& x, BinaryShiftedInt const& y) {
	// TODO: C++20: define just the <=> operator instead of all comparison operators.
	// Not all comparison operators are invoked, but all are written for completeness.
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

    BinaryShiftedInt& operator+=(BinaryShiftedInt const& y) {
	auto minExp = std::min(fExp2, y.fExp2);
        fIntValue <<= fExp2 - minExp;
	fExp2 = minExp;
	fIntValue += y.fIntValue << (y.fExp2 - minExp);
	return *this;
    }
    friend BinaryShiftedInt operator+(BinaryShiftedInt x, BinaryShiftedInt const& y) {
	x += y;
	return x;
    }

    BinaryShiftedInt& operator-=(BinaryShiftedInt const& y) {
	auto minExp = std::min(fExp2, y.fExp2);
        fIntValue <<= fExp2 - minExp;
	fExp2 = minExp;
	fIntValue -= y.fIntValue << (y.fExp2 - minExp);
	return *this;
    }
    friend BinaryShiftedInt operator-(BinaryShiftedInt x, BinaryShiftedInt const& y) {
	x -= y;
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

    void operator<<=(std::int64_t shift) {
	fExp2 += shift;
    }

    void operator>>=(std::int64_t shift) {
	fExp2 -= shift;
    }

    template<Round round>
    BinaryShiftedInt sqrt() {
        if (sign() == Sign::Negative) {
	    throw_exception<PreciseRangeImplLogicError>("sqrt of a negative");
	}
	// ensure exp is even
	if (fExp2 % 2) {
	    --fExp2;
	    fIntValue <<= 1;
	}
	auto intRt = round == Round::TowardsZero ? fIntValue.sqrt() : fIntValue.ceilSqrt();
	return {std::move(intRt), fExp2 / 2};
    }

    friend BinaryShiftedInt distanceToNearestInteger(BinaryShiftedInt x) {
	if (x.fExp2 >= 0) {
	    // Already an int
	    return BinaryShiftedInt(0);
	} else {
	    x.fIntValue.absMe();
	    x.fIntValue %= BigInt(1) << -x.fExp2;
	    BinaryShiftedInt half(1, -1);
	    if (x <= half) {
		return x;
	    } else {
		return BinaryShiftedInt(1) - x;
	    }
	}
    }
    
    std::string toString() const {
	if (fExp2 >= 0) {
	    return static_cast<std::string>(fIntValue << fExp2);
	} else {
	    auto negExp = static_cast<std::size_t>(-fExp2);
	    auto absResult = static_cast<std::string>(fIntValue.abs() * (BigInt(5) ^ negExp));
	    if (absResult.size() <= negExp) {
		absResult = std::string(negExp + 1 - absResult.size(), '0') + absResult;
	    }
	    absResult.insert(absResult.length() - negExp, ".");
	    while (absResult.back() == '0') {
		absResult.pop_back();
	    }
	    if (absResult.back() == '.') {
		absResult.pop_back();
	    }
	    if (fIntValue.neg()) {
		return "-" + absResult;
	    } else {
		return absResult;
	    }
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
	// Use compTruncate to shorten the result bounds to avoid storing many bits beyond necessary
        auto cmp = compTruncate(fLow, fHigh);
	if (auto resultIfDifferent = cmp.fResultIfDifferent) {
	    if (!resultIfDifferent->fFirstArgIsLess) {
		swap(fLow, fHigh);
	    }
	    fUncertaintyLog2 = resultIfDifferent->fCeilLog2Dist;
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

    Optional<RelationToZero> signIfKnown() const {
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
	    return Failure("Sign of range not yet known");
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

    BinaryShiftedInt floorOfUpperBound() const {
	BinaryShiftedInt highCopy(fHigh);
	highCopy.makeFloor();
	return highCopy;
    }

    Optional<std::size_t> maxUpperBoundAbs() const {
	BinaryShiftedIntRange copy(*this);
	auto low = copy.fLow.upperBoundAbs().convert<std::size_t>();
	if (!low) {
	    // failure
	    return low;
	}
	auto high = copy.fHigh.upperBoundAbs().convert<std::size_t>();
	if (!high) {
	    // failure
	    return high;
	}
	return std::max(*low, *high);
    }

    friend BinaryShiftedIntRange operator+(BinaryShiftedIntRange const& x, BinaryShiftedIntRange const& y) {
	return {x.fLow + y.fLow, x.fHigh + y.fHigh};
    }

    template<class Range, class F>
    static BinaryShiftedIntRange sum(Range&& range, F&& f) {
	BinaryShiftedInt low(0);
	BinaryShiftedInt high(0);
	for (auto&& x : range) {
	    BinaryShiftedIntRange term = f(x);
	    low = low + term.fLow;
	    high = high + term.fHigh;
	}
	return {low, high};
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
	    throw_exception<PreciseRangeImplLogicError>("Multiplication without necessary precision should always return a successful result");
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
	    throw_exception<PreciseRangeImplLogicError>("Division denominator includes 0: " + y.toString());
	}
	if (ySignIfKnown->fSign == Sign::Zero) {
	    throw_exception<PreciseRangeImplLogicError>("Division denominator = 0: " + y.toString());
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

    void operator<<=(std::int64_t shift) {
	fLow <<= shift;
	fHigh <<= shift;
	fUncertaintyLog2 = fUncertaintyLog2 + shift;
    }

    void operator>>=(std::int64_t shift) {
	fLow >>= shift;
	fHigh >>= shift;
	fUncertaintyLog2 = fUncertaintyLog2 - shift;
    }

    friend BinaryShiftedIntRange sqrt(BinaryShiftedIntRange x) {
	if (x.fLow.sign() != Sign::Positive) {
	    return {BinaryShiftedInt(0), x.fHigh.sqrt<BinaryShiftedInt::Round::AwayFromZero>()};
	} else {
	    return {x.fLow.sqrt<BinaryShiftedInt::Round::TowardsZero>(), x.fHigh.sqrt<BinaryShiftedInt::Round::AwayFromZero>()};
	}
    }

    friend BinaryShiftedIntRange distanceToNearestInteger(BinaryShiftedIntRange const& x) {
	auto const& low = x.fLow;
	auto const& high = x.fHigh;
	BinaryShiftedInt half(1, -1);
	bool sameSign = low.sign() == high.sign();
	bool containsInt;
	bool containsIntPlusHalf;
	if (sameSign) {
	    containsInt = false;
	    auto biggestBit = std::max(floorLog2Abs(low), floorLog2Abs(high));
	    for (std::int64_t bit = 0; bit <= biggestBit; ++bit) {
		if (low.getBit(bit) != high.getBit(bit)) {
		    containsInt = true;
		    break;
		}
	    }
	    containsIntPlusHalf = false;
	    bool lowCarry = low.getBit(-1);
	    bool highCarry = high.getBit(-1);
	    for (std::int64_t bit = 0; bit-1 <= biggestBit; ++bit) {
		auto rawLowBit = low.getBit(bit);
		auto rawHighBit = high.getBit(bit);
		auto computedLowBit = rawLowBit != lowCarry;
		auto computedHighBit = rawHighBit != highCarry;
		lowCarry = lowCarry && rawLowBit;
		highCarry = highCarry && rawHighBit;
		if (computedLowBit != computedHighBit) {
		    containsIntPlusHalf = true;
		    break;
		}
	    }
	} else {
	    BinaryShiftedInt negHalf(-1, -1);
	    containsInt = true;
	    containsIntPlusHalf = low < negHalf || high > half;
	}
	auto lowDist = distanceToNearestInteger(low);
	auto highDist = distanceToNearestInteger(high);
	if (highDist < lowDist) {
	    // Possible when low=2.5, high=2.6, so lowDist=0.5, highDist=0.4 (must swap)
	    swap(lowDist, highDist);
	}
	if (containsInt) {
	    lowDist = BinaryShiftedInt(0);
	}
	if (containsIntPlusHalf) {
	    highDist = BinaryShiftedInt(1, -1); // 0.5
	}
	return {std::move(lowDist), std::move(highDist)};
    }

    std::string toString() const {
	auto diffToMidpoint = fHigh - fLow;
	diffToMidpoint >>= 1;
	if (!diffToMidpoint) {
	    return fLow.toString();
	}
	auto middle = fLow + diffToMidpoint;
	return "[" + middle.toString() + " ± " + diffToMidpoint.toString() + "]";
    }
};

enum class OperatorPriority : std::uint8_t {
    AdditionSubtraction = 0,    // inside any other expression, requires being shown in ()
    MultiplicationDivision = 1, // inside another * or / or unary fcn or power base, requires being shown in ()
    Power = 2,                  // inside a unary fcn, requires being shown in ()
    ExactValue = 3,             // inside a unary fcn, requires being shown in ()
    RequireParensOnChild = 4,   // force any sub-expression to be shown in ()
};

struct PreciseRange::Impl {
  public:
    virtual ~Impl() = default;

  private:
    virtual OperatorPriority operatorPriority() const = 0;
    virtual std::string toStringExact() const = 0;
    virtual BinaryShiftedIntRange calculateEstimate() = 0;
    virtual BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) = 0;
    virtual OperatorPriority operatorPriorityVsParent() const {
	// Some operators behave with one priority to their children, but a different one to their parent.
	// For example, the unary negation operator -x acts like a RequireParensOnChild to its child
	//   (so the child is always placed in parentheses), -> -(2)
	//   but it acts like MultiplicationDivision to its parent
	//   (so it is usually placed in parentheses) -> (-(2))*3 but 4+-(2)
	// Otherwise
	return operatorPriority();
    }

  public:
    std::string toStringExact(Impl const* parent) const {
	if (parent && parent->operatorPriority() >= operatorPriorityVsParent()) {
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
	    if (fCachedValue->uncertaintyLog2() > maxUncertaintyLog2) {
		// break on the following line to debug calculateUncertaintyLog2AtMost if you hit this branch/exception (indicating a bug in the derived class)
		fCachedValue = calculateUncertaintyLog2AtMost(maxUncertaintyLog2);
		throw_exception<PreciseRangeImplLogicError>("calculateUncertaintyLog2AtMost did not produce requested maxUncertaintyLog2 = " +
							    std::to_string(maxUncertaintyLog2) + " for: " + toStringExact() + " with value " + fCachedValue->toString());
	    }
	}
	return *fCachedValue;
    }
    struct RefinedWithSign {
	BinaryShiftedIntRange const& fRange;
	Sign fSign;
    };
    RefinedWithSign refineUntilSignIsPositiveOrNegative(char const* purpose) {
	auto const& est = estimate();
	if (auto sign = est.signIfKnown()) {
	    if (sign->fSign == Sign::Zero) {
		throw_exception<std::domain_error>(std::string("Invalid value of 0 for ") + purpose + ": " + toStringExact());
	    }
	    return {est, sign->fSign};
	}
	// Dereference is safe: if there was no uncertainty, the sign would be known.
	std::int64_t const estimateUncertaintyLog2 = *est.uncertaintyLog2().fValue;
	auto uncertaintyLog2 = estimateUncertaintyLog2;
	for (std::int64_t extraPrecision = 1; extraPrecision <= 1024; extraPrecision *= 2) {
	    uncertaintyLog2 = estimateUncertaintyLog2 - extraPrecision;
	    BinaryShiftedIntRange const& result = withUncertaintyLog2AtMost(uncertaintyLog2);
	    if (auto sign = result.signIfKnown()) {
		if (sign->fSign == Sign::Zero) {
		    throw_exception<std::domain_error>(std::string("Invalid value of 0 for ") + purpose + ": " + toStringExact());
		}
		return {result, sign->fSign};
	    }
	}
	throw_exception<std::domain_error>(std::string("For ") + purpose + ", with uncertainty ≤ 2^(" + std::to_string(uncertaintyLog2) + "), could not determine sign of " + toStringExact());
    }

    RefinedWithSign refineUntilSignIsPositiveOrNegativeAndRangeInFactorOf2(char const* purpose) {
        RefinedWithSign refinedWithSign = refineUntilSignIsPositiveOrNegative(purpose);
	auto maximumLog2Uncertainty = *minFloorLog2Abs(refinedWithSign.fRange);
	if (refinedWithSign.fRange.uncertaintyLog2() <= maximumLog2Uncertainty) {
	    return refinedWithSign;
	}
	return {withUncertaintyLog2AtMost(maximumLog2Uncertainty), refinedWithSign.fSign};
    }

  private:
    std::optional<BinaryShiftedIntRange> fCachedValue;
};

struct ExactValue : public PreciseRange::Impl {
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

/* Usage example: SquarePlusOne
struct SquarePlusOne : public ImplAsAnotherRangeWithBase<SquarePlusOne, PreciseUnaryOp>
    using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;
    PreciseRange calculateImpl() {
        return fArg * fArg + 1;
    }
    std::string op() const final {
        return "squarePlusOne";
    }
};
*/
// The derived class MUST implement a calculateImpl() method that returns PreciseRange.
// It will only be called once, the first time estimate() or withUncertaintyLog2AtMost() is called.
template<class Self, class Base>
struct ImplAsAnotherRangeWithBase : public Base {
    static_assert(std::is_base_of_v<PreciseRange::Impl, Base>, "Base class should be PreciseRange::Impl or something inheriting from it");
  public:
    using Base::Base;

  private:
    PreciseRange::Impl& impl() {
	if (!fImpl) {
	    fImpl = static_cast<Self&>(*this).calculateImpl().impl();
	}
	return *fImpl;
    }

    BinaryShiftedIntRange calculateEstimate() final {
	return impl().estimate();
    }

    BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	return impl().withUncertaintyLog2AtMost(maxUncertaintyLog2);
    }

  private:
    PreciseRange::SharedImpl fImpl;
};

struct PreciseUnaryOp : public PreciseRange::Impl {
    PreciseRange fArg;
    PreciseRange::Impl& fArgImpl;

    // Necessary for derived classes to be able to "using PreciseUnaryOp::PreciseUnaryOp;"
    PreciseUnaryOp(PreciseRange arg)
	: fArg(arg)
	, fArgImpl(*fArg.impl()) {}
    
    virtual std::string op() const = 0;
    OperatorPriority operatorPriority() const final {
	return OperatorPriority::RequireParensOnChild;
    }
    std::string toStringExact() const final {
	return op() + fArgImpl.toStringExact(this);
    }
};

struct PreciseBinaryOp : public PreciseRange::Impl {
    PreciseRange fLhs;
    PreciseRange fRhs;
    PreciseRange::Impl& fLhsImpl;
    PreciseRange::Impl& fRhsImpl;

    // Necessary for derived classes to be able to "using PreciseBinaryOp::PreciseBinaryOp;"
    PreciseBinaryOp(PreciseRange lhs, PreciseRange rhs)
	: fLhs(lhs)
	, fRhs(rhs)
	, fLhsImpl(*fLhs.impl())
	, fRhsImpl(*fRhs.impl()) {}

    virtual std::string op() const = 0;
    std::string toStringExact() const final {
	return fLhsImpl.toStringExact(this) + op() + fRhsImpl.toStringExact(this);
    }
};

PreciseRange::PreciseRange(std::int64_t value)
    : fImpl(std::make_shared<ExactValue>(BigInt(value))) {}

static PreciseRange fromNumeric(std::string_view numericLiteral) {
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
	    PreciseRange(std::make_shared<ExactValue>(std::move(numerator))) /
	    PreciseRange(std::make_shared<ExactValue>(std::move(denominator)));
	return fraction;
    }
}

PreciseRange::PreciseRange(std::string_view numericLiteral)
    : PreciseRange(fromNumeric(numericLiteral)) {}

PreciseRange::PreciseRange(SharedImpl nonNullImpl /* throws if null */)
    : fImpl(std::move(nonNullImpl)) {
    if (!fImpl) {
	throw_exception<std::invalid_argument>("PreciseRange cannot be constructed from null");
    }
}

PreciseRange::~PreciseRange() = default;

PreciseRange::SharedImpl const& PreciseRange::impl() const {
    return fImpl;
}

PreciseRange PreciseRange::π() {
    // What is π?
    // First we must define sin and cos:
    // cos(x) = ∑_{i=0}^{∞} (-1)^i * x^(2*i) / (2*i)!
    // sin(x) = ∑_{i=0}^{∞} (-1)^i * x^(2*i+1) / (2*i+1)!
    // Then we can define π:
    // π = 2 * (the smallest positive x where cos(x) = 0)

    // It can be seen that sin and cos converge absolutely by comparing with a geometric series after i ≥ |x|.

    // cos(0) = 1 and sin(0) = 0 by substitution.

    // cos^2(x) + sin^2(x)
    //  = ∑_{i=0}^{∞} (-1)^i * x^(2*i) / (2*i)! * ∑_{j=0}^{∞} (-1)^j * x^(2*j) / (2*j)!
    //    + ∑_{i=0}^{∞} (-1)^i * x^(2*i+1) / (2*i+1)! * ∑_{j=0}^{∞} (-1)^j * x^(2*j+1) / (2*j+1)!
    //  = ∑_{i=0}^{∞} ∑_{j=0}^{∞} (-1)^(i+j) * x^(2*(i+j)) / (2*i)! / (2*j)!
    //    + ∑_{i=0}^{∞} ∑_{j=0}^{∞} (-1)^(i+j) * x^(2*(i+j+1)) / (2*i+1)! / (2*j+1)!
    //  = ∑_{i=0}^{∞} ∑_{s=i}^{∞} (-1)^(s) * x^(2*(s)) / (2*i)! / (2*(s-i))!
    //    + ∑_{i=0}^{∞} ∑_{s=i}^{∞} (-1)^(s) * x^(2*(s+1)) / (2*i+1)! / (2*(s-i)+1)!
    //  = ∑_{s=0}^{∞} ∑_{i=0}^{s} (-1)^(s) * x^(2*s) / (2*i)! / (2*(s-i))!
    //    + ∑_{s=0}^{∞} ∑_{i=0}^{s} (-1)^(s) * x^(2*(s+1)) / (2*i+1)! / (2*(s-i)+1)!
    //  = 1
    //    + ∑_{s=1}^{∞} ∑_{i=0}^{s} (-1)^(s) * x^(2*s) / (2*i)! / (2*(s-i))!
    //    + ∑_{s=0}^{∞} ∑_{i=0}^{s} (-1)^(s) * x^(2*(s+1)) / (2*i+1)! / (2*(s-i)+1)!
    //  = 1
    //    + ∑_{s=0}^{∞} ∑_{i=0}^{s+1} (-1)^(s+1) * x^(2*(s+1)) / (2*i)! / (2*(s+1-i))!
    //    + ∑_{s=0}^{∞} ∑_{i=0}^{s} (-1)^(s) * x^(2*(s+1)) / (2*i+1)! / (2*(s-i)+1)!
    //  = 1
    //    + ∑_{s=0}^{∞} (-1)^(s+1) * x^(2*(s+1)) * ( ∑_{i=0}^{s+1} 1 / (2*i)! / (2*(s+1-i))! - ∑_{i=0}^{s} 1 / (2*i+1)! / (2*(s-i)+1)! )
    // We will show that ∑_{i=0}^{s+1} 1 / (2*i)! / (2*(s+1-i))! = ∑_{i=0}^{s} 1 / (2*i+1)! / (2*(s-i)+1)!
    // ∑_{i=0}^{s+1} 1 / (2*i)! / (2*(s+1-i))!
    //  = 1 / (2*s+2) * ∑_{i=0}^{s+1} (2*s+2) / (2*i)! / (2*(s+1-i))!
    //  = 1 / (2*s+2) * [2 / (2*s+1)! + ∑_{i=1}^{s} (2*s+2) / (2*i)! / (2*(s+1-i))!]
    //  = 1 / (2*s+2) * [2 / (2*s+1)! + ∑_{i=1}^{s} (2*i+2*s+2-2*i) / (2*i)! / (2*(s+1-i))!]
    //  = 1 / (2*s+2) * [2 / (2*s+1)! + ∑_{i=1}^{s} (2*i) / (2*i)! / (2*(s+1-i))! + (2*s+2-2*i) / (2*i)! / (2*(s+1-i))!]
    //  = 1 / (2*s+2) * [2 / (2*s+1)! + ∑_{i=1}^{s} 1 / (2*i-1)! / (2*(s+1-i))! + 1 / (2*i)! / (2*s+1-2*i)!]
    //  = 1 / (2*s+2) * [2 / (2*s+1)! + ∑_{i=1}^{s} 1 / (2*i-1)! / (2*(s+1-i))! + ∑_{j=1}^{s} 1 / (2*j)! / (2*s+1-2*j)!]
    //  = 1 / (2*s+2) * [∑_{i=1}^{s+1} 1 / (2*i-1)! / (2*(s+1-i))! + ∑_{j=0}^{s} 1 / (2*j)! / (2*s+1-2*j)!]
    //  = 1 / (2*s+2) * [∑_{i=0}^{s} 1 / (2*i+1)! / (2*(s-i))! + ∑_{j=0}^{s} 1 / (2*j)! / (2*s+1-2*j)!]
    //  = 1 / (2*s+2) * [∑_{i=0}^{s} 1 / (2*i+1)! / (2*(s-i))! + 1 / (2*i)! / (2*s+1-2*i)!]
    //  = 1 / (2*s+2) * [∑_{i=0}^{s} (2*s-2*i+1) / (2*i+1)! / (2*s-2*i+1)! + (2*i+1) / (2*i+1)! / (2*s+1-2*i)!]
    //  = 1 / (2*s+2) * ∑_{i=0}^{s} (2*s-2*i+1+2*i+1) / (2*i+1)! / (2*s-2*i+1)!
    //  = 1 / (2*s+2) * ∑_{i=0}^{s} (2*s+2) / (2*i+1)! / (2*s-2*i+1)!
    //  = ∑_{i=0}^{s} 1 / (2*i+1)! / (2*s-2*i+1)!
    //  = ∑_{i=0}^{s} 1 / (2*i+1)! / (2*(s-i)+1)!
    // Thus cos^2(x) + sin^2(x) = 1 + ∑_{s=0}^{∞} (-1)^(s+1) * x^(2*(s+1)) * 0 = 1.

    // cos(x) is continuous - look up a ε-δ proof online.

    // cos(2)
    //  = ∑_{i=0}^{∞} (-1)^i * 2^(2*i) / (2*i)!
    //  = 1 - 2^2/2! + 2^4/4! - (alternating sign terms that decrease in magnitude)
    //  < 1 - 2^2/2! + 2^4/4!
    //  < 1 - 2 + 16/24
    //  < -1/3
    //  < 0

    // Since cos is continuous and cos(0) = 1 and cos(2) < 0, ∃x with 0<x<2, cos(x) = 0.
    // sin(x) on (0,2] is positive, since ∑_{i=0}^{∞} (-1)^i * x^(2*i+1) / (2*i+1)! is alternating sign terms that decrease in amplitude (ratio of x^2/2/3, x^2/4/5, ...)
    // (d/dx) cos(x) = -sin(x) by observation, so cos(x) is decreasing on [0,2].
    // Therefore there is exactly one root of cos(x)=0 in [0,2].
    // We define π as 2 * this root.
    struct Pi : public PreciseRange::Impl {
      private:
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::ExactValue;
	}
	std::string toStringExact() const final {
	    return "π";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return withUncertaintyLog2AtMost(-2);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    // Could do something more efficient like:
	    // https://en.wikipedia.org/wiki/Gauss%E2%80%93Legendre_algorithm
	    // https://en.wikipedia.org/wiki/Pi#Computer_era_and_iterative_algorithms
	    // but there's some niceness to defining the algorithm outselves.
	    while (true) {
		BinaryShiftedIntRange range(fLow, fHigh);
		if (range.uncertaintyLog2() <= maxUncertaintyLog2) {
		    return range;
		}
		auto midpoint = fLow + fHigh;
		midpoint >>= 1; // approx π
		auto sq = midpoint * midpoint;
		sq >>= 2; // approx (π/2)^2
		BinaryShiftedInt val{0}; // cos(midpoint/2) with a positive multiplier
		// val = (2*k)! * ∑_{i=0}^{k} (-1)^i x^(2*i) / (2*i)!
		// Each loop iteration increments 2 by 2.
		BinaryShiftedInt pow{1};
		for (UI exp = 0; true; exp += 4) {
		    val += pow;
		    if (val.sign() == Sign::Negative) {
			// if we're negative after a positive term, we will always be negative
			// Thus cos(midpoint) < 0, so low < π < midpoint
			fHigh = std::move(midpoint);
			break;
		    }
		    val = val * BinaryShiftedInt(exp + 1);
		    val = val * BinaryShiftedInt(exp + 2);
		    pow = pow * sq;

		    val -= pow;
		    if (val.sign() == Sign::Positive) {
			// if we're positive after a negative term, we will always be positive
			// Thus cos(midpoint) > 0, so midpoint < π < high
			fLow = std::move(midpoint);
			break;
		    }
		    val = val * BinaryShiftedInt(exp + 3);
		    val = val * BinaryShiftedInt(exp + 4);
		    pow = pow * sq;
		}
	    }
	}

      private:
	BinaryShiftedInt fLow{0};
	BinaryShiftedInt fHigh{4};
    };
    static thread_local auto pi = std::make_shared<Pi>(); // cache via static local var
    return pi;
}

PreciseRange PreciseRange::operator-() const {
    struct Negative : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	OperatorPriority operatorPriorityVsParent() const final {
	    return OperatorPriority::MultiplicationDivision;
	}
	std::string op() const final {
	    return "-";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    auto result = fArgImpl.estimate();
	    result.negateMe();
	    return result;
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto result = fArgImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2);
	    result.negateMe();
	    return result;
	}
    };
    return std::make_shared<Negative>(*this);
}

PreciseRange sqrt(PreciseRange const& x) {
    // For math competitions:
    // √(n) ≈ (n + nearest_square) / (2  * √nearest_square)
    // since, squaring both sides,
    // n ≈ (n + nearest_square)^2 / (4 * nearest_square)
    //   = (n^2 + 2*n*nearest_square + nearest_square^2) / (4 * nearest_square)
    //   = n + (n^2 - 2*n*nearest_square + nearest_square^2) / (4 * nearest_square)
    //   = n + (n - nearest_square)^2 / (4 * nearest_square)
    // which is close to n as n/nearest_square is close to 1
    // https://en.wikipedia.org/wiki/Integer_square_root
    // https://stackoverflow.com/questions/10725522/arbitrary-precision-of-square-roots
    struct Sqrt : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "√";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    auto argEst = fArgImpl.estimate();
	    if (auto sign = argEst.signIfKnown(); sign && sign->fSign == Sign::Negative) {
		throw_exception<std::domain_error>("sqrt of a negative: " + toStringExact());
	    }
	    return sqrt(argEst);
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto argEst = fArgImpl.estimate();
	    if (auto sign = argEst.signIfKnown(); sign && sign->fSign == Sign::Positive) {
		// We seek k so that, for any X,Y satisfying
		// 0 < A ≤ X ≤ Y ≤ B, Y-X ≤ 2^k,
		// √Y-√X ≤ 2^(maxUncertaintyLog2-1)
		// √Y-√X
		//  = (Y-X)/(√Y+√X)
		//  ≤ (Y-X)/(2√A)
		//  ≤ (Y-X)/2^(1+⌊log2(A)⌋/2)
		//  ≤ (Y-X)/2^(1+⌊⌊log2(A)⌋/2⌋)
		//  ≤ 2^k/2^(1+⌊⌊log2(A)⌋/2⌋)
		//  = 2^(k-1-⌊⌊log2(A)⌋/2⌋)
		// We choose k ≤ maxUncertaintyLog2 + ⌊⌊log2(A)⌋/2⌋ so...
		//  ≤ 2^(maxUncertaintyLog2-1)
		// However, note that 2*(maxUncertaintyLog2-1) is also sufficient (see below),
		//   so we choose k = max(maxUncertaintyLog2 + ⌊⌊log2(A)⌋/2⌋, 2*(maxUncertaintyLog2-1))
		auto floorLog2LowerBound = *minFloorLog2Abs(argEst);
		if (floorLog2LowerBound % 2) {
		    // so we always round down when dividing by 2
		    --floorLog2LowerBound;
		}
		auto k = std::max(maxUncertaintyLog2 + floorLog2LowerBound / 2, 2 * (maxUncertaintyLog2 - 1));
		argEst = fArgImpl.withUncertaintyLog2AtMost(k);
	    } else {
		// We seek k so that, for any X,Y satisfying
		// 0 ≤ X ≤ Y, Y-X ≤ 2^k,
		// √Y-√X ≤ 2^(maxUncertaintyLog2-1)
		// √Y-√X
		//  = √(Y-2√(XY)+X)
		//  ≤ √(Y-2√(XX)+X)
		//  = √(Y-X)
		//  ≤ √(2^k)
		//  = 2^(k/2)
		// We choose k ≤ 2*(maxUncertaintyLog2-1) so...
		//  ≤ 2^(maxUncertaintyLog2-1)
		argEst = fArgImpl.withUncertaintyLog2AtMost(2 * (maxUncertaintyLog2 - 1));
	    }
	    if (auto sign = argEst.signIfKnown(); sign && sign->fSign == Sign::Negative) {
		throw_exception<std::domain_error>("sqrt of a negative: " + toStringExact());
	    }
	    // lowRoundErr ≤ 2^(lowExp/2) ≤ 2^(maxUncertaintyLog2 - 2)
	    // highRoundErr ≤ 2^(highExp/2) ≤ 2^(maxUncertaintyLog2 - 2)
	    // totalRoundErr = lowRoundErr + highRoundErr ≤ 2^(maxUncertaintyLog2 - 1)
	    // totalErr = totalRoundErr + exactErr ≤ 2^maxUncertaintyLog2
	    argEst.shiftToHaveExponentsLeq(2 * (maxUncertaintyLog2 - 2));
	    return sqrt(argEst);
	}
    };
    return std::make_shared<Sqrt>(x);
}

PreciseRange distanceToNearestInteger(PreciseRange const& x) {
    struct DistanceToNearestInteger : public PreciseUnaryOp {
	using PreciseUnaryOp::PreciseUnaryOp;
	std::string op() const final {
	    return "distanceToNearestInteger";
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    return distanceToNearestInteger(fArgImpl.estimate());
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return distanceToNearestInteger(fArgImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2));
	}
    };
    return std::make_shared<DistanceToNearestInteger>(x);
}

enum class PowerSeriesCoefficients { InvExpFactorial /* 1/n! */, InvExp /* 1/n */ };

// Provide the derivatives at 0: f(0), f'(0) f''(0), ...
// These are assumed to repeat, and must only be -1, 0, and 1.
// For example:
// e^x has f(0) = f'(0) = f''(0) = ... = 1, so it corresponds to <1>
// sin(x) has f(0) = 0, f'(0) = 1, f''(0) = 0, f'''(0) = -1, ..., so it corresponds to <0, 1, 0, -1>
// As one might expect, this is expensive for large inputs, and can blow out the stack based on the input value.
// Try to compute a power series with a smaller input, and perform additional operations to reach the final result.
// For example, e^x can be computed as (e^(x/2^k))^(2^k) for any chosen value of k, where ^(2^k) can be performed via multiplication.
template<PowerSeriesCoefficients CoefficientsKind, std::int64_t... Derivatives>
struct PowerSeriesOp : public PreciseUnaryOp {
    using PreciseUnaryOp::PreciseUnaryOp;

    static_assert(((-1 <= Derivatives && Derivatives <= 1) && ...), "Power series only supports repeated derivatives of -1, 0, and 1");
    static_assert(((Derivatives != 0) || ...), "Power series must have at least one nonzero term");

    static constexpr std::array DerivativesArray = {Derivatives...};

    PreciseRange computeNewTermWithoutCoefficient(std::int64_t exp) const {
	if constexpr (CoefficientsKind == PowerSeriesCoefficients::InvExpFactorial) {
	    // Emperical evidence for large power series (x=300) shows this multiplication + division order produces faster code than other orders
	    return fArg / exp * fTermsWithoutCoefficients.back();
	} else {
	    (void)exp;
	    return fArg * fTermsWithoutCoefficients.back();
	}
    }

    PreciseRange computeNewTermToSumEithoutDerivativeMultiplier(PreciseRange newTermWithoutCoefficient, std::int64_t exp) const {
	if constexpr (CoefficientsKind == PowerSeriesCoefficients::InvExpFactorial) {
	    (void)exp;
	    return newTermWithoutCoefficient;
	} else {
	    return newTermWithoutCoefficient / exp;
	}
    }

    void addTerm() {
	auto exp = fTermsWithoutCoefficients.size();
	// Emperical evidence for large power series (x=300) shows this multiplication + division order produces faster code than other orders
	auto newTermWithoutCoefficient = computeNewTermWithoutCoefficient(static_cast<std::int64_t>(exp));
	fTermsWithoutCoefficients.push_back(newTermWithoutCoefficient);
	if (auto d = DerivativesArray[exp % DerivativesArray.size()]) {
	    fTermsToSum.push_back(fFinalTermToSum);
	    auto termToSumWithoutDerivativeMultiplier = computeNewTermToSumEithoutDerivativeMultiplier(newTermWithoutCoefficient, static_cast<std::int64_t>(exp));
	    if (d == 1) {
		fFinalTermToSum = termToSumWithoutDerivativeMultiplier;
	    } else if (d == -1) {
		fFinalTermToSum = -termToSumWithoutDerivativeMultiplier;
	    } else {
		throw_exception<PreciseRangeImplLogicError>("Power series derivatives should only be -1, 0, or 1, but encountered " + std::to_string(d) + " in '" + op() + "'");
	    }
	}
    }

    void ensureInit() {
	if (fTermsToSum.empty()) {
	    // get input within a range of size 2^0 = 1 to get an accurate upper bound
	    auto optBound = fArgImpl.withUncertaintyLog2AtMost(0).maxUpperBoundAbs();
	    if (!optBound) {
		throw_exception<std::domain_error>("Power series is too large (more than " + std::to_string(std::numeric_limits<std::size_t>::max()) + " elements) for " + toStringExact());
	    }
	    auto bound = *optBound;
	    auto minExpNeeded = 2u * bound;
	    while (fTermsWithoutCoefficients.size() < minExpNeeded) {
		addTerm();
	    }
	    // get a nonzero term whose exponent guarantees later terms will be less in absolute value by at least a factor of 2
	    // This guarantees that the sum of all remaining terms is at most 2 * finalTerm,
	    //   which allows us to bound the uncertainty in the result.
	    auto numSummedTerms = fTermsToSum.size();
	    while (fTermsToSum.size() == numSummedTerms) {
		addTerm();
	    }
	}
    }

    BinaryShiftedIntRange calculate(std::optional<std::int64_t> maxUncertaintyLog2) {
	ensureInit();
	if (maxUncertaintyLog2) {
	    auto uncertaintyLog2ForTermsSum = *maxUncertaintyLog2 - 1;
	    auto uncertaintyLog2ForRemainingTerms = *maxUncertaintyLog2 - 1;
	    // finalTerm * [0,2] will have uncertainty ≤ 4*2^(maxCeilLog2Abs(finalTerm)) which we want ≤ 2^uncertaintyLog2ForRemainingTerms
	    auto upperBoundLog2ForFinalTerm = uncertaintyLog2ForRemainingTerms - 2;
	    auto calculateFinalTerm = [&, this]() { return fFinalTermToSum.impl()->withUncertaintyLog2AtMost(upperBoundLog2ForFinalTerm - 1); };
	    while (maxCeilLog2Abs(calculateFinalTerm()) > upperBoundLog2ForFinalTerm) {
		addTerm();
	    }
	    // If we're summing 5 terms and need their sum to be accurate within 2^k, compute each term to be accurate within
	    //   2^(k-⌈log2(5)⌉) = 2^(k-3)
	    auto ceilLog2NumTermsInSum = static_cast<std::int64_t>(std::ceil(std::log2(fTermsToSum.size())));
	    auto uncertaintyLog2PerTermInSum = uncertaintyLog2ForTermsSum - ceilLog2NumTermsInSum;
	    // Use BinaryShiftedIntRange::sum instead of BinaryShiftedIntRange::operator+ to avoid intermediate rounding
	    return BinaryShiftedIntRange::sum(fTermsToSum, [&](PreciseRange& r) { return r.impl()->withUncertaintyLog2AtMost(uncertaintyLog2PerTermInSum); }) + calculateFinalTerm() * BinaryShiftedIntRange(BinaryShiftedInt(0), BinaryShiftedInt(2));
	} else {
	    // Use BinaryShiftedIntRange::sum instead of BinaryShiftedIntRange::operator+ to avoid intermediate rounding
	    return BinaryShiftedIntRange::sum(fTermsToSum, [](PreciseRange& r) { return r.impl()->estimate(); }) + fFinalTermToSum.impl()->estimate() * BinaryShiftedIntRange(BinaryShiftedInt(0), BinaryShiftedInt(2));
	}
    }

    BinaryShiftedIntRange calculateEstimate() final {
	return calculate(std::nullopt);
    }

    BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	return calculate(maxUncertaintyLog2);
    }

  private:
    std::vector<PreciseRange> fTermsWithoutCoefficients{1}; // [i] = x^i/i!
    std::vector<PreciseRange> fTermsToSum;
    PreciseRange fFinalTermToSum{CoefficientsKind == PowerSeriesCoefficients::InvExpFactorial ? DerivativesArray[0] : std::int64_t(0)};
};

PreciseRange exp(PreciseRange const& x) {
    struct RawExp : public PowerSeriesOp<PowerSeriesCoefficients::InvExpFactorial, 1> {
	using PowerSeriesOp::PowerSeriesOp;
	OperatorPriority operatorPriorityVsParent() const final {
	    return OperatorPriority::Power;
	}
	std::string op() const final {
	    return "e^";
	}
    };

    struct ExpWithPreScaling : public ImplAsAnotherRangeWithBase<ExpWithPreScaling, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    // Choose k so |x| ≤ 2^k, 0 ≤ k
	    // Choose k = max(⌈log2(|x|)⌉, 0) + 3 so each term of the power series will be reduced by at least a factor of 8, for extra-fast convergence.
	    // However, even larger values of k can cost (with the final k squarings) more than they gain in power series convergence.
	    auto k = *std::max(maxCeilLog2Abs(fArgImpl.withUncertaintyLog2AtMost(0)), IntOrNegInf(0)) + 3;
	    // y = x / 2^k, so |y| ≤ 1. This makes the power series of y converge quickly.
	    auto y = fArg * std::make_shared<ExactValue>(BinaryShiftedInt(1, -k));
	    // e^x = e^(x / 2^k * 2*k) = (e^(x / 2^k))^(2^k) = (e^y)^(2^k)
	    // e^y:
	    PreciseRange result = std::make_shared<RawExp>(y);
	    // (e^y)^(2^k)
	    for (auto i = k; i--; ) {
		result = result * result;
	    }
	    return result;
	}
	OperatorPriority operatorPriorityVsParent() const final {
	    return OperatorPriority::Power;
	}
	std::string op() const final {
	    return "e^";
	}
    };
    // This divides the input by 2^k to get a value < 1, then raises the result to the 2^k'th power.
    // This is much more efficient than the direct power series computation:
    // return std::make_shared<RawExp>(x);
    return std::make_shared<ExpWithPreScaling>(x);
}

PreciseRange ln(PreciseRange const& x) {
    // Construct a power series for ∑_{i=1}^{∞} x^i/i
    struct LnHelper : public PowerSeriesOp<PowerSeriesCoefficients::InvExp, 1> {
	using PowerSeriesOp::PowerSeriesOp;
	std::string op() const final {
	    return "neg_shift_ln_impl";
	}
    };

    struct LnWithPreScaling : public ImplAsAnotherRangeWithBase<LnWithPreScaling, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

	static PreciseRange negLnOneMinusX(PreciseRange arg) {
	    // ln(x) = -∑_{i=1}^{∞} (1-x)^i/i when 0 < x ≤ 2
	    // negLnOneMinusX(x) = -ln(1-x) = ∑_{i=1}^{∞} x^i/i
	    return PreciseRange(std::make_shared<LnHelper>(arg));
	}

        PreciseRange calculateImpl() {
	    // ln(x * 2^y) = ln(x) - y*ln(1/2) = y * negLnOneMinusX(1/2) - negLnOneMinusX(1-x)
	    // First refine x within a factor of 2.
	    // Then select y = ⌈log2(x)⌉ to shift x into [1/4, 1].
	    auto const& [argVal, sign] = fArgImpl.refineUntilSignIsPositiveOrNegativeAndRangeInFactorOf2("ln");
	    if (sign != Sign::Positive) {
		throw_exception<std::domain_error>("ln of a non-positive number: " + toStringExact());
	    }
	    auto y = *maxCeilLog2Abs(argVal);
	    auto x = fArg >> y;
	    auto half = std::make_shared<ExactValue>(BinaryShiftedInt(1, -1));
	    return y * negLnOneMinusX(half) - negLnOneMinusX(1 - x);
	}
	std::string op() const final {
	    return "ln";
	}
    };
    return std::make_shared<LnWithPreScaling>(x);
}

// Here we prove sin(x+y) = sin(x)*cos(y) + cos(x)*sin(y)
// sin(x+y)
//  = ∑_{i=0}^{∞} (-1)^i (x+y)^(2*i+1) / (2*i+1)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i+1} choose(2*i+1,j) * x^j * y^(2*i+1-j) / (2*i+1)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i+1} (2*i+1)!/j!/(2*i+1-j)! * x^j * y^(2*i+1-j) / (2*i+1)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i+1} x^j / j! * y^(2*i+1-j) / (2*i+1-j)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{i} x^(2*j) / (2*j)! * y^(2*i+1-2*j) / (2*i+1-2*j)! + ∑_{k=0}^{i} x^(2*k+1) / (2*k+1)! * y^(2*i+1-2*k-1) / (2*i+1-2*k-1)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{i} x^(2*j) / (2*j)! * y^(2*i+1-2*j) / (2*i+1-2*j)! + ∑_{k=0}^{i} x^(2*k+1) / (2*k+1)! * y^(2*i-2*k) / (2*i-2*k)!
//  = ∑_{j=0}^{∞} ∑_{i=j}^{∞} (-1)^i x^(2*j) / (2*j)! * y^(2*i+1-2*j) / (2*i+1-2*j)! + ∑_{k=0}^{∞} ∑_{i=k}^{∞} (-1)^i x^(2*k+1) / (2*k+1)! * y^(2*i-2*k) / (2*i-2*k)!
//  = ∑_{j=0}^{∞} ∑_{i=0}^{∞} (-1)^(i+j) x^(2*j) / (2*j)! * y^(2*i+1) / (2*i+1)! + ∑_{k=0}^{∞} ∑_{i=0}^{∞} (-1)^(i+k) x^(2*k+1) / (2*k+1)! * y^(2*i) / (2*i)!
//  = ∑_{j=0}^{∞} (-1)^j x^(2*j) / (2*j)! * ∑_{i=0}^{∞} (-1)^i y^(2*i+1) / (2*i+1)! + ∑_{k=0}^{∞} (-1)^k x^(2*k+1) / (2*k+1)! * ∑_{i=0}^{∞} (-1)^i y^(2*i) / (2*i)!
//  = cos(x) * sin(y) + sin(x) * cos(y)

// Here we prove cos(x+y) = cos(x)*cos(y) - sin(x)*sin(y)
// cos(x+y)
//  = ∑_{i=0}^{∞} (-1)^i (x+y)^(2*i) / (2*i)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i} choose(2*i,j) * x^j * y^(2*i-j) / (2*i)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i} (2*i)!/j!/(2*i-j)! * x^j * y^(2*i-j) / (2*i)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{2*i} x^j / j! * y^(2*i-j) / (2*i-j)!
//  = ∑_{i=0}^{∞} (-1)^i ∑_{j=0}^{i} x^(2*j) / (2*j)! * y^(2*i-2*j) / (2*i-2*j)! + ∑_{k=0}^{i-1} x^(2*k+1) / (2*k+1)! * y^(2*i-2*k-1) / (2*i-2*k-1)!
//  = ∑_{j=0}^{∞} ∑_{i=j}^{∞} (-1)^i x^(2*j) / (2*j)! * y^(2*i-2*j) / (2*i-2*j)! + ∑_{k=0}^{∞} ∑_{i=k+1}^{∞} (-1)^i x^(2*k+1) / (2*k+1)! * y^(2*i-2*k-1) / (2*i-2*k-1)!
//  = ∑_{j=0}^{∞} ∑_{i=0}^{∞} (-1)^(i+j) x^(2*j) / (2*j)! * y^(2*i) / (2*i)! + ∑_{k=0}^{∞} ∑_{i=0}^{∞} (-1)^(i+k+1) x^(2*k+1) / (2*k+1)! * y^(2*i+1) / (2*i+1)!
//  = ∑_{j=0}^{∞} (-1)^j x^(2*j) / (2*j)! * ∑_{i=0}^{∞} (-1)^i y^(2*i) / (2*i)! - ∑_{k=0}^{∞} (-1)^k x^(2*k+1) / (2*k+1)! * ∑_{i=0}^{∞} (-1)^i y^(2*i+1) / (2*i+1)!
//  = cos(x) * cos(y) - sin(x) * sin(y)

// sin(π) = sin(π/2 + π/2) = sin(π/2)*cos(π/2) + cos(π/2)*sin(π/2) = 1*0 + 0*1 = 0
// cos(π) = cos(π/2 + π/2) = cos(π/2)*cos(π/2) - sin(π/2)*sin(π/2) = 0*0 - 1*1 = -1

// sin(2*π) = sin(π + π) = sin(π)*cos(π) + cos(π)*sin(π) = 0*-1 + -1*0 = 0
// cos(2*π) = cos(π + π) = cos(π)*cos(π) - sin(π)*sin(π) = -1*-1 - 0*0 = 1

PreciseRange sin(PreciseRange const& x) {
    struct SinRaw : public PowerSeriesOp<PowerSeriesCoefficients::InvExpFactorial, 0, 1, 0, -1> {
	using PowerSeriesOp::PowerSeriesOp;
	std::string op() const final {
	    return "sin";
	}
    };
    // Compute the argument mod 2π before performing the power series calculation.
    // This is much more efficient than a direct power series computation.
    // sin(x + 2*π) = sin(x)*cos(2*π) + cos(x)*sin(2*π) = sin(x)*1 + cos(x)*0 = sin(x)
    // Therefore sin(x + k*2*π) = sin(x) for any integer k.
    struct SinWithPreMod : public ImplAsAnotherRangeWithBase<SinWithPreMod, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    return std::make_shared<SinRaw>(mod(fArg, PreciseRange::π() << 1));
	}
	std::string op() const final {
	    return "sin";
	}
    };
    return std::make_shared<SinWithPreMod>(x);
}

PreciseRange cos(PreciseRange const& x) {
    struct CosRaw : public PowerSeriesOp<PowerSeriesCoefficients::InvExpFactorial, 1, 0, -1, 0> {
	using PowerSeriesOp::PowerSeriesOp;
	std::string op() const final {
	    return "cos";
	}
    };
    // Compute the argument mod 2π before performing the power series calculation.
    // This is much more efficient than a direct power series computation.
    // cos(x + 2*π) = cos(x)*cos(2*π) - sin(x)*sin(2*π) = cos(x)*1 - sin(x)*0 = cos(x)
    // Therefore cos(x + k*2*π) = cos(x) for any integer k.
    struct CosWithPreMod : public ImplAsAnotherRangeWithBase<CosWithPreMod, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    return std::make_shared<CosRaw>(mod(fArg, PreciseRange::π() << 1));
	}
	std::string op() const final {
	    return "cos";
	}
    };
    return std::make_shared<CosWithPreMod>(x);
}

PreciseRange sinh(PreciseRange const& x) {
    struct Sinh : public ImplAsAnotherRangeWithBase<Sinh, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    // This is much more efficient than a direct power series computation.
	    return (exp(fArg) - exp(-fArg)) >> 1;
	}

	std::string op() const final {
	    return "sinh";
	}
    };
    /*
    struct Sinh : public PowerSeriesOp<PowerSeriesCoefficients::InvExpFactorial, 0, 1> {
	using PowerSeriesOp::PowerSeriesOp;
	std::string op() const final {
	    return "sinh";
	}
    };
    */
    return std::make_shared<Sinh>(x);
}

PreciseRange cosh(PreciseRange const& x) {
    struct Cosh : public ImplAsAnotherRangeWithBase<Cosh, PreciseUnaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    // This is much more efficient than a direct power series computation.
	    return (exp(fArg) + exp(-fArg)) >> 1;
	}

	std::string op() const final {
	    return "cosh";
	}
    };
    /*
    struct Cosh : public PowerSeriesOp<PowerSeriesCoefficients::InvExpFactorial, 1, 0> {
	using PowerSeriesOp::PowerSeriesOp;
	std::string op() const final {
	    return "cosh";
	}
    };
    */
    return std::make_shared<Cosh>(x);
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
	    return fLhsImpl.estimate() + fRhsImpl.estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return fLhsImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1) + fRhsImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1);
	}
    };
    return std::make_shared<Sum>(x, y);
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
	    return fLhsImpl.estimate() - fRhsImpl.estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    return fLhsImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1) - fRhsImpl.withUncertaintyLog2AtMost(maxUncertaintyLog2 - 1);
	}
    };
    return std::make_shared<Difference>(x, y);
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
	    return fLhsImpl.estimate() * fRhsImpl.estimate();
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto productOrNecessaryArgUncertainty = productWithDesiredEstimate(fLhsImpl.estimate(), fRhsImpl.estimate(), maxUncertaintyLog2);
	    auto [asProduct, asNecessaryArgUncertainty] = productOrNecessaryArgUncertainty.split();
	    if (asProduct) {
		return std::move(*asProduct);
	    } else {
		auto lhsMoreCertain = fLhsImpl.withUncertaintyLog2AtMost(asNecessaryArgUncertainty->fMaxLhsUncertaintyLog2);
		auto rhsMoreCertain = fRhsImpl.withUncertaintyLog2AtMost(asNecessaryArgUncertainty->fMaxRhsUncertaintyLog2);
		return lhsMoreCertain * rhsMoreCertain;
	    }
	}
    };
    return std::make_shared<Product>(x, y);
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
	    return fLhsImpl.estimate() / fRhsImpl.refineUntilSignIsPositiveOrNegative("division").fRange;
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
	    auto const& denominator = fRhsImpl.refineUntilSignIsPositiveOrNegativeAndRangeInFactorOf2("division").fRange;
	    if (auto result = fLhsImpl.estimate() / denominator; result.uncertaintyLog2() <= maxUncertaintyLog2) {
		// result from estimate is good enough!
		return result;
	    } else if (auto resultSign = result.signIfKnown()) {
		// Numerator and denominator both have known sign -> dispatch to this case
		auto log2AbsResultUpperBound = ceilLog2Abs(resultSign->fFurtherFromZero);
		return calculateUncertaintyLog2AtMostWhereBothOperandsSignsKnown(maxUncertaintyLog2, log2AbsResultUpperBound);
	    }
	    
	    // It seems like the numerator is close to 0 (since the result sign isn't known).
	    // We determine a numerator refinement that either:
	    // A. if the numerator remains on both sides of 0, produces a result satisfying the desired precision
	    // B. refines the numerator to one side of 0, where we can dispatch to the "both known sign" case.
	    // For (A), we seek k so that, for Q,R satisfying a ≤ Q ≤ 0 ≤ R ≤ b, R-Q ≤ 2^k,
	    // R/C-Q/C ≤ 2^(maxUncertaintyLog2-1)
	    // R/C-Q/C
	    //  = (R-Q)/C
	    //  ≤ 2^k/C
	    //  ≤ 2^k/2^⌊log2(C)⌋
	    //  = 2^(k-⌊log2(C)⌋)
	    // We choose k = ⌊log2(C)⌋ + maxUncertaintyLog2 - 1
	    auto k = *minFloorLog2Abs(denominator) + maxUncertaintyLog2 - 1;
	    auto refinedNumerator = fLhsImpl.withUncertaintyLog2AtMost(k);
	    if (auto refinedNumeratorSign = refinedNumerator.signIfKnown()) {
		// Case (B): numerator is on one side of 0
		auto log2AbsResultUpperBound = ceilLog2Abs(refinedNumeratorSign->fFurtherFromZero) - *minFloorLog2Abs(denominator);
		return calculateUncertaintyLog2AtMostWhereBothOperandsSignsKnown(maxUncertaintyLog2, log2AbsResultUpperBound);
	    }

	    // Case (A): numerator is still close to 0.
	    // Next we limit the uncertainty when performing rounding during division.
	    // Specifically, the uncertainty is at most 2^(numeratorExp-denominatorExp)
	    // We need this to be ≤ 2^(maxUncertaintyLog2-2) on each end, so the uncertainty from rounding both ends is ≤ 2^(maxUncertaintyLog2-1)
	    // Thus we require numeratorExp-denominatorExp ≤ maxUncertaintyLog2-2, so:
	    // numeratorExp ≤ maxUncertaintyLog2-2+denominatorExp
	    refinedNumerator.shiftToHaveExponentsLeq(maxUncertaintyLog2 - 2 + denominator.minExp());
	    auto result = refinedNumerator / denominator;
	    if (result.uncertaintyLog2() <= maxUncertaintyLog2) {
		return result;
	    } else {
		throw_exception<PreciseRangeImplLogicError>("Division for numerator without a sign failed to meet expected precision: " + refinedNumerator.toString() + " / " + denominator.toString() + " = " + result.toString() + " has uncertaintyLog2 = " + result.uncertaintyLog2().toString() + " >= " + std::to_string(maxUncertaintyLog2) + " (desired)");
	    }
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMostWhereBothOperandsSignsKnown(std::int64_t maxUncertaintyLog2, IntOrNegInf log2AbsResultUpperBound) {
	    // numerator and denominator each lay on a specific side of 0.
	    // We seek k so that, for any Q,R,S,T satisfying A ≤ Q ≤ R ≤ B, R-Q ≤ 2^k, C ≤ S ≤ T ≤ D, T-S ≤ 2^k,
	    // R/S-Q/T ≤ 2^(maxUncertaintyLog2-1)
	    // Also let E = B/C = upper bound of abs(result)
	    // R/S-Q/T
	    //  ≤ (Q+2^k)/(T-2^k)-Q/T (since R ≤ Q+2^k and S ≥ T-2^k)
	    //  = (T*(Q+2^k)-Q*(T-2^k))/(T*(T-2^k))
	    //  = (T*2^k+Q*2^k)/(T*(T-2^k))
	    //  ≤ (1+Q/T)/(T-2^k)*2^k
	    //  ≤ (1+B/C)/(C-2^k)*2^k
	    //  = (1+E)/(C-2^k)*2^k
	    // We choose k ≤ log2(C)-1 so 2^k ≤ C/2, and C-2^k ≥ C/2, so...
	    //  ≤ (1+E)/C*2^(k+1)
	    //  ≤ (1+2^⌈log2(E)⌉)/(2^⌊log2(C)⌋)*2^(k+1)
	    //  = (2^0+2^⌈log2(E)⌉)/(2^⌊log2(C)⌋)*2^(k+1)
	    //  ≤ 2^(1+max(0,⌈log2(E)⌉))/(2^⌊log2(C)⌋)*2^(k+1)
	    //  = 2^(max(0,⌈log2(E)⌉)-⌊log2(C)⌋+k+2)
	    // We choose k ≤ ⌊log2(C)⌋-3-max(0,⌈log2(E)⌉)+maxUncertaintyLog2
	    //  ≤ 2^(maxUncertaintyLog2-1)
	    // Thus we can select
	    // k = min(⌊log2(C)⌋-1, ⌊log2(C)⌋-3-max(0,⌈log2(E)⌉)+maxUncertaintyLog2)
	    //   = ⌊log2(C)⌋-1 + min(0, maxUncertaintyLog2-2-max(0,⌈log2(E)⌉))
	    std::int64_t zero = 0;
	    auto k = *minFloorLog2Abs(fRhsImpl.estimate()) - 1 + std::min(zero, maxUncertaintyLog2 - 2 - *std::max(IntOrNegInf(zero), log2AbsResultUpperBound));
	    auto numerator = fLhsImpl.withUncertaintyLog2AtMost(k);
	    auto const& denominator = fRhsImpl.withUncertaintyLog2AtMost(k);
	    // Next we limit the uncertainty when performing rounding during division.
	    // Specifically, the uncertainty is at most 2^(numeratorExp-denominatorExp)
	    // We need this to be ≤ 2^(maxUncertaintyLog2-2) on each end, so the uncertainty from rounding both ends is ≤ 2^(maxUncertaintyLog2-1)
	    // Thus we require numeratorExp-denominatorExp ≤ maxUncertaintyLog2-2, so:
	    // numeratorExp ≤ maxUncertaintyLog2-2+denominatorExp
	    numerator.shiftToHaveExponentsLeq(maxUncertaintyLog2 - 2 + denominator.minExp());
	    // The uncertainty from the rounding on both ends combined is at most 2^(maxUncertaintyLog2-1)
	    // The uncertainty in the resulting range is at most 2^(maxUncertaintyLog2-1)
	    // Thus the total uncertainty is at most 2^maxUncertaintyLog2
	    return numerator / denominator;
	    
	}
    };
    return std::make_shared<Quotient>(x, y);
}

PreciseRange mod(PreciseRange const& x, PreciseRange const& y) {
    struct Mod : public ImplAsAnotherRangeWithBase<Mod, PreciseBinaryOp> {
	using ImplAsAnotherRangeWithBase::ImplAsAnotherRangeWithBase;

        PreciseRange calculateImpl() {
	    PreciseRange div = fLhs / fRhs;
	    PreciseRange multiplier{std::make_shared<ExactValue>(div.impl()->withUncertaintyLog2AtMost(0).floorOfUpperBound())};
	    return fLhs - fRhs * multiplier;
	}

	std::string op() const final {
	    return " mod ";
	}

	OperatorPriority operatorPriority() const final {
	    // Always display children in ()
	    return OperatorPriority::RequireParensOnChild;
	}

	OperatorPriority operatorPriorityVsParent() const final {
	    // Always display this in ()
	    return OperatorPriority::AdditionSubtraction;
	}
    };
    return std::make_shared<Mod>(x, y);
}

PreciseRange operator^(PreciseRange const& x, std::int64_t exponent) {
    struct Power : public ImplAsAnotherRangeWithBase<Power, PreciseRange::Impl> {
      public:
	Power(PreciseRange base, std::int64_t exponent)
	    : fBase(base)
	    , fExponent(exponent) {}

	PreciseRange calculateImpl() {
	    PreciseRange basePow = fBase;
	    std::optional<PreciseRange> result;
	    for (auto e = my_abs(fExponent); e; e /= 2, basePow = basePow * basePow) {
		if (e % 2) {
		    if (!result) {
			result = basePow;
		    } else {
			// Emperical evidence for large power series (x=2000) shows either multiplication order has around the same performance
			result = basePow * *result;
		    }
		}
	    }
	    if (result) {
		if (fExponent < 0) {
		    return 1 / *result;
		} else {
		    return *result;
		}
	    } else {
		return 1;
	    }
	}

      private:
	OperatorPriority operatorPriority() const final {
	    return OperatorPriority::Power;
	}
	std::string toStringExact() const final {
	    return fBase.impl()->toStringExact(this) + "^" + std::to_string(fExponent);
	}

      private:
        PreciseRange fBase;
	std::int64_t fExponent;
    };
    return std::make_shared<Power>(x, exponent);
}

PreciseRange operator<<(PreciseRange const& x, std::int64_t shift) {
    struct LeftShift : public PreciseRange::Impl {
      public:
	LeftShift(PreciseRange value, std::int64_t shift)
	    : fValue(value.impl())
	    , fShift(shift) {}

      private:
	OperatorPriority operatorPriority() const final {
	    // Always display this in ()
	    return OperatorPriority::AdditionSubtraction;
	}
	std::string toStringExact() const final {
	    return fValue->toStringExact(this) + "<<" + std::to_string(fShift);
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    auto est = fValue->estimate();
	    est <<= fShift;
	    return est;
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto val = fValue->withUncertaintyLog2AtMost(maxUncertaintyLog2 - fShift);
	    val <<= fShift;
	    return val;
	}

      private:
        PreciseRange::SharedImpl fValue;
	std::int64_t fShift;
    };
    return std::make_shared<LeftShift>(x, shift);
}

PreciseRange operator>>(PreciseRange const& x, std::int64_t shift) {
    struct RightShift : public PreciseRange::Impl {
      public:
	RightShift(PreciseRange value, std::int64_t shift)
	    : fValue(value.impl())
	    , fShift(shift) {}

      private:
	OperatorPriority operatorPriority() const final {
	    // Always display this in ()
	    return OperatorPriority::AdditionSubtraction;
	}
	std::string toStringExact() const final {
	    return fValue->toStringExact(this) + ">>" + std::to_string(fShift);
	}
	BinaryShiftedIntRange calculateEstimate() final {
	    auto est = fValue->estimate();
	    est >>= fShift;
	    return est;
	}
	BinaryShiftedIntRange calculateUncertaintyLog2AtMost(std::int64_t maxUncertaintyLog2) final {
	    auto val = fValue->withUncertaintyLog2AtMost(maxUncertaintyLog2 + fShift);
	    val >>= fShift;
	    return val;
	}

      private:
        PreciseRange::SharedImpl fValue;
	std::int64_t fShift;
    };
    return std::make_shared<RightShift>(x, shift);
}

PreciseRange::Cmp cmp(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2) {
    auto sub = (x - y).impl();
    auto estimate = sub->estimate();
    if (auto sign = estimate.signIfKnown()) {
	return signToCmp(sign->fSign);
    }
    auto estimateUncertaintyLog2 = *estimate.uncertaintyLog2(); // deref is safe: if no uncertainty, sign would be known
    if (estimateUncertaintyLog2 <= maxUncertaintyLog2) {
	return PreciseRange::Cmp::ApproxEqual;
    }
    // Try with exponentially more accurate results.
    // Say we estimated with precision ≤ 2^5, but we need precision ≤ 2^(-20).
    // Try with precision ≤ 2^(5-1), 2^(5-2), 2^(5-4), 2^(5-8), 2^(5-16), and finally 2^(-20)
    auto neededUncertaintyReductionLog2 = estimateUncertaintyLog2 - maxUncertaintyLog2;
    for (std::int64_t uncertaintyReductionLog2 = 1; uncertaintyReductionLog2 < neededUncertaintyReductionLog2; uncertaintyReductionLog2 *= 2) {
	if (auto sign = sub->withUncertaintyLog2AtMost(estimateUncertaintyLog2 - uncertaintyReductionLog2).signIfKnown()) {
	    return signToCmp(sign->fSign);
	}
    }
    // last try
    if (auto sign = sub->withUncertaintyLog2AtMost(maxUncertaintyLog2).signIfKnown()) {
	return signToCmp(sign->fSign);
    }
    return PreciseRange::Cmp::ApproxEqual;
}

bool eq(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2) {
    return cmp(x, y, maxUncertaintyLog2) == PreciseRange::Cmp::ApproxEqual;
}

std::string PreciseRange::toStringExact() const {
    return impl()->toStringExact(nullptr);
}

std::string PreciseRange::toStringWithUncertaintyLog2AtMost(std::optional<std::int64_t> maxUncertaintyLog2) const {
    return impl()->toStringWithUncertaintyLog2AtMost(maxUncertaintyLog2);
}
