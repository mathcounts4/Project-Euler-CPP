#include "BigInt.hpp"
#include "Class.hpp"
#include "DigitParser.hpp"
#include "ExitUtils.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

// private data:
std::string const BigInt::INF_STR = "INF";
std::string const BigInt::NEG_INF_STR = "-INF";

class BigInt::Impl {
  public:
    static constexpr auto shift_sz = BigInt::shift_sz;

    enum class AddOrSubtract { Add, Subtract };
    template<AddOrSubtract action>
    static void addOrSubtract(BigInt& x, BigInt const& y) {
	B matchesSign = (x.negative == y.negative) == (action == AddOrSubtract::Add);
	if (x.inf()) {
	    if (y.inf() && !matchesSign) {
		throw_exception<std::domain_error>(static_cast<std::string>(x) +
						   (action == AddOrSubtract::Add ? " + " : " -") +
						   static_cast<std::string>(y));
	    }
	    return;
	}
	if (y.inf()) {
	    x = y;
	    if (action == AddOrSubtract::Subtract) {
		x.negateMe();
	    }
	    return;
	}
	if (matchesSign) {
	    // same sign -> addition -> carry indicates add 1
	    B carry = false;
	    SZ i = 0;
	    if (x.data.size() < y.data.size()) {
		x.data.resize(y.data.size(), small_t(0));
	    }
	    for (; i < y.data.size(); ++i) {
		small_t d = x.data[i];
		if (carry) {
		    carry = 0 == ++d;
		}
		small_t res = d + y.data[i];
		carry |= res < d;
		x.data[i] = res;
	    }
	    while (carry) {
		if (i == x.data.size()) {
		    x.data.emplace_back(1);
		    break;
		} else {
		    carry = 0 == ++x.data[i++];
		}
	    }
	} else {
	    if (lessThanInAbs(x, y)) {
		// TODO: it'd be nice to avoid recursion and copying y
		BigInt withYFirst(y);
		addOrSubtract<action>(withYFirst, x);
		if (action == AddOrSubtract::Subtract) {
		    // other - this -> this - other
		    withYFirst.negateMe();
		}
		x.swap(withYFirst);
		return;
	    }

	    // different signs -> subtraction -> carry indicates subtract 1
	    B carry = false;
	    SZ i = 0;
	    for (; i < y.data.size(); ++i) {
		if (carry) {
		    carry = 0 == x.data[i]--;
		}
		carry |= x.data[i] < y.data[i];
		x.data[i] -= y.data[i];
	    }
	    if (carry) {
		while (0 == x.data[i++]--); // intentional ;
	    }
	    x.reduce();
	}
    }

    enum class DivRemKind { DivTowardsZero, DivAwayFromZero, Rem };
    template<DivRemKind kind>
    static void divOrRem(BigInt& x, BigInt const& y) {
	constexpr B IsRem = kind == DivRemKind::Rem;
	constexpr B IsDiv = kind != DivRemKind::Rem;
	constexpr B RoundAway = kind == DivRemKind::DivAwayFromZero;
	if (!y) {
	    throw_exception<std::domain_error>("div/rem by zero");
	}
	if (x.inf() && y.inf()) {
	    throw_exception<std::domain_error>("±Inf div/rem ±Inf");
	}
	if (!x) {
	    return;
	}
	auto toAddIfBetweenInts = RoundAway ? x.neg() == y.neg() ? 1 : -1 : 0;
	if constexpr (IsDiv) {
	    if (y.neg()) {
		// For the rest of this function, we ignore the sign of other.
		// *this has the final sign (with a final reduce() at the end, if it becomes -0).
		x.negateMe();
	    }
	}
	if (x.inf()) {
	    if constexpr (IsRem) {
		throw_exception<std::domain_error>("±Inf rem x");
	    }
	    return;
	}
	if (y.inf()) {
	    if constexpr (IsRem) {
		// don't modify x (remainder mod ±inf)
	    } else {
		// Already checked for x=0 above, so we have a remainder to round away from 0
		x = toAddIfBetweenInts;
	    }
	    return;
	}

	if (y.isPowerOf2()) {
	    SZ k = y.log2();
	    if constexpr (IsRem) {
		// x % 2^k
		SZ data_sz = (k + shift_sz - 1) / shift_sz;
		if (x.data.size() > data_sz) {
		    x.data.resize(data_sz);
		}
		SZ small_bits = k % shift_sz;
		if (small_bits > 0 && x.data.size() == data_sz) {
		    x.data.back() &= (small_t(1) << small_bits) - small_t(1);
		}
		x.reduce();
		return;
	    } else {
		B betweenInts = false;
		if constexpr (RoundAway) {
		    // x / 2^k -> check last k bits
		    if (x.data.size() > k / shift_sz &&
			(x.data[k / shift_sz] & ((small_t(1) << (k % shift_sz)) - small_t(1)))) {
			// last word of x that contains some of the final k bits
			// check the last (k % shift_sz) bits of this word
			betweenInts = true;
		    } else {
			for (auto word = std::min(k / shift_sz, x.data.size()); word--; ) {
			    if (x.data[word]) {
				betweenInts = true;
				break;
			    }
			}
		    }
		}
		x >>= static_cast<SL>(k);
		if (betweenInts) {
		    x += toAddIfBetweenInts;
		}
		return;
	    }
	}

	if (y.data.size() == 1) {
	    big_t value = 0;
	    small_t divisor = y.data[0];
	    for (SZ i = x.data.size(); i-- > 0; ) {
		value = (value << shift_sz) | x.data[i];
		if constexpr (IsDiv) {
		    x.data[i] = static_cast<small_t>(value / divisor);
		}
		value %= divisor;
	    }
	    if constexpr (IsRem) {
		B const is_neg = x.neg();
		x = value;
		if (is_neg) {
		    x.negateMe();
		}
	    } else {
		x.reduce();
		if (value) {
		    x += toAddIfBetweenInts;
		}
	    }
	    return;
	}

	SZ shift;
	{
	    auto xl2 = x.log2();
	    auto yl2 = y.log2();
	    if (xl2 < yl2) {
		if constexpr (IsDiv) {
		    x = toAddIfBetweenInts;
		}
		return;
	    }
	    shift = xl2 - yl2;
	}
	B resultNegative = x.neg();
	x.absMe();

	BigInt divResult;
	if constexpr (IsDiv) {
	    divResult.data.resize(shift / shift_sz + 1);
	}

	BigInt times = y.abs();
	times <<= static_cast<SL>(shift);
	for (++shift; shift--; times >>= 1) {
	    if (x >= times) {
		x -= times;
		if constexpr (IsDiv) {
		    divResult.data[shift / shift_sz] |= small_t(1) << (shift % shift_sz);
		}
	    }
	}
	if constexpr (IsRem) {
	    x.negative = resultNegative && x ? Negative::True : Negative::False;
	} else {
	    B isBetweenInts = static_cast<B>(x);
	    x.swap(divResult);
	    x.reduce();
	    x.negative = resultNegative && x ? Negative::True : Negative::False;
	    if (isBetweenInts) {
		x += toAddIfBetweenInts;
	    }
	}
    }

    enum class RtRound { Floor, Ceil };
    template<RtRound round>
    static BigInt sqrtBigInt(BigInt const& x) {
	if (!x) {
	    return x;
	}
	if (x.neg()) {
	    throw_exception<std::domain_error>("BigInt sqrt called on " + to_string(x));
	}
	if (x.inf()) {
	    return x;
	}
	SZ log2rt = x.log2() / 2;
	// 2^log2rt is a lower bound on the square root, and 2^(log2rt+1) is a strict upper bound on the square root.
	BigInt rt = BigInt(1) << static_cast<SL>(log2rt);
	// Now we see which smaller bits should be flipped to 1 in the result.

	// Iterate from largest to smallest bit, checking if (rt + bit)^2 ≤ x
	// (rt + bit)^2 = rt^2 + 2*rt*bit + bit*bit
	// store x-rt*rt and compare with 2*rt*bit + bit*bit
	BigInt remaining = x - rt * rt;
	SZ log2LastBit = log2rt;
	while (log2LastBit && remaining) {
	    // Jump down bits by calculating an upper bound on the next possible bit:
	    // 2*rt*bit < remaining (since bit*bit > 0)
	    // bit < remaining / (2 * rt)
	    //     ≤ 2^(⌈log2(remaining)⌉) / (2 * 2^(⌊log2(rt)⌋))
	    //     = 2^(⌈log2(remaining)⌉ - 1 - ⌊log2(rt)⌋)
	    auto pc = remaining.ceilLog2();
	    auto nc = log2rt + 1;
	    if (pc <= nc) {
		break;
	    }
	    bool success = false;
	    for (SZ bitLog2 = std::min(pc - nc, log2LastBit); bitLog2--; ) {
		auto additional = (rt << static_cast<SL>(bitLog2+1)) + (BigInt(1) << static_cast<SL>(bitLog2*2));
		if (remaining >= additional) {
		    remaining -= additional;
		    rt.data[bitLog2/shift_sz] += small_t(1) << bitLog2%shift_sz;
		    log2LastBit = bitLog2;
		    success = true;
		    break;
		}
	    }
	    if (!success) {
		break;
	    }
	}
	if (round == RtRound::Ceil && remaining) {
	    ++rt;
	}
	return rt;
    }
};

// deletes extra 0's from the data
void BigInt::reduce() {
    while (data.size() > 0 && data.back() == 0)
	data.pop_back();
    if (data.size() == 0)
	negative = Negative::False;
}

// returns if this is +- a power of 2
B BigInt::isPowerOf2() const {
    if (!data.size())
	return false;
    if (!is_pow_2(data.back()))
	return false;
    for (SZ i = data.size()-1; i--; )
	if (data[i] > 0)
	    return false;
    return true;
}

// floor(log2(abs(this)))
SZ BigInt::log2() const {
    if (data.size()) {
	return static_cast<SZ>(std::log2(data.back())) + shift_sz * (data.size() - 1);
    }
    throw_exception<std::domain_error>("BigInt log2 called on " + to_string(*this));
}

SZ BigInt::ceilLog2() const {
    auto result = log2();
    if (!isPowerOf2()) {
	++result;
    }
    return result;
}

// floor(abs(this)/2^n) % 2
B BigInt::getBit(SZ n) const {
    if (n / shift_sz < data.size()) {
	return ((data[n / shift_sz] >> (n % shift_sz)) & 1u) == 1u;
    } else {
	return false;
    }
}

// floor(sqrt(this))
BigInt BigInt::sqrt() const {
    return BigInt::Impl::sqrtBigInt<BigInt::Impl::RtRound::Floor>(*this);
}

// ceil(sqrt(this))
BigInt BigInt::ceilSqrt() const {
    return BigInt::Impl::sqrtBigInt<BigInt::Impl::RtRound::Ceil>(*this);
}

// constructors
BigInt::BigInt()
    : infinite(Infinite::False)
    , negative(Negative::False) {
}
BigInt::BigInt(std::string const& s) {
    *this = *from_string<BigInt>(s);
}
BigInt::BigInt(Infinite inf, Negative neg)
    : infinite(inf)
    , negative(neg) {
}

// infinite constants
BigInt const& BigInt::Inf() {
    static BigInt const result{Infinite::True, Negative::False};
    return result;
}
BigInt const& BigInt::NegInf() {
    static BigInt const result{Infinite::True, Negative::True};
    return result;
}

// infinite B functions
B BigInt::inf() const {
    return infinite == Infinite::True;
}
B BigInt::neg() const {
    return negative == Negative::True;
}
SI BigInt::cmp() const {
    if (!*this) {
	return 0;
    }
    return neg() ? -1 : 1;
}
B lessThanInAbs(BigInt const& x, BigInt const& y) {
    if (x.inf()) {
	return false;
    }
    if (y.inf()) {
	return true;
    }
    if (!y) {
	return false;
    }
    if (!x) {
	return true;
    }
    auto sx = x.data.size();
    auto sy = y.data.size();
    if (sx != sy) {
	return sx < sy;
    }
    for (auto i = sx; i--; ) {
	if (x.data[i] != y.data[i]) {
	    return x.data[i] < y.data[i];
	}
    }
    return false;
}

// swap
void swap(BigInt& x, BigInt& y) {
    std::swap(x.infinite, y.infinite);
    std::swap(x.negative, y.negative);
    std::swap(x.data, y.data);
}
void BigInt::swap(BigInt& other) {
    ::swap(*this, other);
}


// type conversion operators
BigInt::operator bool() const {
    return data.size() || inf();
}
BigInt::operator std::string() const {
    if (inf())
	return neg() ? NEG_INF_STR : INF_STR;
    
    std::string result;
    BigInt copy = abs();
    
    do {
	result += '0' + *(copy % 10).convert<C>();
	copy /= 10;
    } while (B(copy));
	    
    if (neg())
	result += "-";
	    
    std::reverse(result.begin(),result.end());
    return result;
}

// comparison operators
B operator<(BigInt const& x, BigInt const& y) {
    auto scale = [](BigInt const& b) -> SL {
	return (b.neg() ? -1 : 1) *
	SL(b.inf() ? 1L << 60 : b.data.size());
    };

    if (scale(x) != scale(y))
	return scale(x) < scale(y);
    
    for (SZ i = x.data.size(); i--; )
	if (x.data[i] != y.data[i])
	    return (x.data[i] < y.data[i]) ^ x.neg();
    return false;
}
B operator>(BigInt const& x, BigInt const& y) { return y < x; }
B operator<=(BigInt const& x, BigInt const& y) { return !(y < x); }
B operator>=(BigInt const& x, BigInt const& y) { return !(x < y); }
B operator==(BigInt const& x, BigInt const& y) {
    return x.infinite == y.infinite && x.negative == y.negative && x.data == y.data;
}
B operator!=(BigInt const& x, BigInt const& y) { return !(x == y); }

// arithmetic operators
BigInt& BigInt::absMe() {
    negative = Negative::False;
    return *this;
}
BigInt BigInt::abs() const { BigInt copy(*this); copy.absMe(); return copy; }
BigInt& BigInt::negateMe() {
    if (*this) {
	if (neg()) {
	    negative = Negative::False;
	} else {
	    negative = Negative::True;
	}
    }
    return *this;
}
BigInt BigInt::operator-() const { return BigInt(*this).negateMe(); }

BigInt& BigInt::operator++() { return *this += 1; }
BigInt& BigInt::operator--() { return *this -= 1; }

BigInt& BigInt::operator+=(BigInt const& o) {
    Impl::addOrSubtract<Impl::AddOrSubtract::Add>(*this, o);
    return *this;
}
BigInt& BigInt::operator-=(BigInt const& o) {
    Impl::addOrSubtract<Impl::AddOrSubtract::Subtract>(*this, o);
    return *this;
}
BigInt& BigInt::operator*=(BigInt const& other) {
    if (!*this) {
	return *this;
    }
    if (!other) {
	return *this = other;
    }
    
    if (inf() || other.inf()) {
	infinite = Infinite::True;
	if (other.neg()) {
	    negateMe();
	}
	data.clear();
	return *this;
    }
    
    if (other.isPowerOf2()) {
	if (other.neg()) {
	    negateMe();
	}
	return *this <<= static_cast<SL>(other.log2());
    }
    
    if (isPowerOf2()) {
	auto pow = log2();
	bool const wasNeg = neg();
	*this = other;
	if (wasNeg) {
	    negateMe();
	}
	return *this <<= static_cast<SL>(pow);
    }
    
    if (other.neg()) {
	negateMe();
    }
    BigInt result{Infinite::False, negative};
    auto& rdata = result.data;
    auto const& odata = other.data;
    SZ size = data.size();
    SZ osize = odata.size();
	    
    rdata.resize(size + osize - 1, 0);

    // TODO:
    // https://en.wikipedia.org/wiki/Karatsuba_algorithm
    // https://www.youtube.com/watch?v=AMl6EJHfUWo

    big_t current = 0;
    big_t carry = 0;
    for (SZ new_d = 0; new_d < osize; ++new_d) {
	current = carry & low_digits;
	carry >>= shift_sz;
	for (SZ i = 0; i < size && i <= new_d; ++i) {
	    // This cannot overflow, as current + x * y ≤ max_small_t + max_small_t * max_small_t
	    current += big_t(data[i]) * big_t(odata[new_d-i]);
	    carry += current >> shift_sz;
	    current &= low_digits;
	}
	rdata[new_d] = static_cast<small_t>(current);
    }
    for (SZ new_d = osize; new_d + 1 < size + osize; ++new_d) {
	current = carry & low_digits;
	carry >>= shift_sz;
	for (SZ i = new_d - osize + 1; i < size && i <= new_d; ++i) {
	    // This cannot overflow, as current + x * y ≤ max_small_t + max_small_t * max_small_t
	    current += big_t(data[i]) * big_t(odata[new_d-i]);
	    carry += current >> shift_sz;
	    current &= low_digits;
	}
	rdata[new_d] = static_cast<small_t>(current);
    }
    while (carry) {
	rdata.push_back(carry & low_digits);
	carry >>= shift_sz;
    }

    result.reduce();
    swap(result);
    return *this;
}
BigInt& BigInt::operator/=(BigInt const& other) {
    Impl::divOrRem<Impl::DivRemKind::DivTowardsZero>(*this, other);
    return *this;
}
// always has the same sign as *this (unless becomes 0)
BigInt& BigInt::operator%=(BigInt const& other) {
    Impl::divOrRem<Impl::DivRemKind::Rem>(*this, other);
    return *this;
}
BigInt divideRoundAwayFrom0(BigInt x, BigInt const& y) {
    BigInt::Impl::divOrRem<BigInt::Impl::DivRemKind::DivAwayFromZero>(x, y);
    return x;
}
BigInt& BigInt::operator^=(std::size_t exp) {
    if (!exp && !*this)
	throw_exception<std::invalid_argument>("BigInt: 0^0");
    BigInt result(1);
    for ( ; exp; ) {
	if (exp & 1)
	    result *= *this;
	exp >>= 1;
	if (exp)
	    *this *= *this;
    }
    return *this = std::move(result);
}
BigInt& BigInt::operator<<=(SL shift) {
    if (inf() || shift == 0 || !*this)
	return *this;
    
    if (shift < 0) {
	if (-shift < 0)
	    throw_exception<std::invalid_argument>(
		to_string(shift) +
		" passed as << argument to BigInt");
	return *this >>= -shift;
    }
    UL ushift = static_cast<UL>(shift);
    SZ small_shift = ushift % shift_sz;
    SZ num_terms_shift = ushift / shift_sz;
    if (small_shift == 0) {
	data.insert(data.begin(),num_terms_shift,0);
    } else {
	SZ const rev_shift = shift_sz - small_shift;
	small_t const another_term = data.back() >> rev_shift;
	B const has_another_term = another_term;
	SZ const old_sz = data.size();
    
	data.resize(old_sz + num_terms_shift + has_another_term,0);
	if (has_another_term)
	    data.back() = another_term;
	for (SZ i = old_sz; --i > 0; )
	    data[i + num_terms_shift] =
		(data[i] << small_shift) |
		(data[i-1] >> rev_shift);
	data[num_terms_shift] = data[0] << small_shift;
	for (SZ i = num_terms_shift; i-- > 0; )
	    data[i] = 0;
    }
    return *this;
}
BigInt& BigInt::operator>>=(SL shift) {
    if (inf() || shift == 0 || !*this)
	return *this;
	    
    if (shift < 0) {
	if (-shift < 0)
	    throw_exception<std::invalid_argument>(
		to_string(shift) +
		" passed as >> argument to BigInt");
	return *this <<= -shift;
    }
    UL ushift = static_cast<UL>(shift);
    SZ const small_shift = ushift % shift_sz;
    SZ const num_terms_shift = ushift / shift_sz;
    if (small_shift == 0) {
	if (num_terms_shift >= data.size()) {
	    *this = 0;
	} else {
	    data.erase(data.begin(),data.begin()+static_cast<SL>(num_terms_shift));
	}
    } else {
	SZ const rev_shift = shift_sz - small_shift;
	B const lose_another_term = (data.back() >> small_shift) == 0;

	if (num_terms_shift + lose_another_term >= data.size()) {
	    *this = 0;
	} else {
	    SZ const num_terms_left = data.size() - num_terms_shift - lose_another_term;

	    for (SZ i = 0; i < num_terms_left-1; ++i)
		data[i] = (data[i+num_terms_shift] >> small_shift) |
		    (data[i+num_terms_shift+1] << rev_shift);
	    data[num_terms_left-1] = data[num_terms_left-1+num_terms_shift] >> small_shift;
	    if (lose_another_term)
		data[num_terms_left-1] |=
		    data[num_terms_left+num_terms_shift] << rev_shift;

	    data.resize(num_terms_left);
	}
    }
    
    return *this;
}

BigInt operator+(BigInt x, BigInt const& y) { x += y; return x; }
BigInt operator-(BigInt x, BigInt const& y) { x -= y; return x; }
BigInt operator*(BigInt x, BigInt const& y) { x *= y; return x; }
BigInt operator/(BigInt x, BigInt const& y) { x /= y; return x; }
BigInt operator%(BigInt x, BigInt const& y) { x %= y; return x; }
BigInt BigInt::operator^(std::size_t exp) const { return BigInt(*this) ^= exp; }
BigInt BigInt::operator<<(SL const l) const { return BigInt(*this) <<= l; }
BigInt BigInt::operator>>(SL const l) const { return BigInt(*this) >>= l; }

// Class<BigInt>
template<> std::ostream& Class<BigInt>::print(std::ostream& oss, BigInt const& bi) {
    return oss << std::string(bi);
}

template<> Optional<BigInt> Class<BigInt>::parse(std::istream& is) {
    Resetter resetter{is};
    std::ws(is);
    
    if (consume(is,BigInt::INF_STR)) {
	resetter.ignore();
	return BigInt::Inf();
    } else if (consume(is,BigInt::NEG_INF_STR)) {
	resetter.ignore();
	return BigInt::NegInf();
    }	    
    
    B const negative = is.peek() == '-';
    if (negative) {
	is.get();
    }

    BigInt result;
    B init = false;
    DigitParser digitParser = DecimalParser;
    if (consume(is, '0')) {
	if (auto parserFromPrefix = DigitParser::fromPrefixChar(is.peek())) {
	    digitParser = *parserFromPrefix;
	    is.get();
	} else {
	    init = true;
	}
    }
    while (is) {
	C const next = static_cast<C>(is.peek());
	if (auto nextDigit = digitParser.fCharToDigit(next)) {
	    init = true;
	    result *= digitParser.fMultPerDigit;
	    result += *nextDigit;
	    is.get();
	} else {
	    break;
	}
    }

    if (!init) {
	return Failure("No digits encountered for BigInt");
    }

    if (negative && result) {
	result.negateMe();
    }
    
    resetter.ignore();
    return result;
}

template<> std::string Class<BigInt>::name() {
    return "BigInt";
}

template<> std::string Class<BigInt>::format() {
    return "[-]?[0-9]{1+}";
}
