#include "BigInt.hpp"
#include "Class.hpp"
#include "ExitUtils.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

// private data:
std::string const BigInt::INF_STR = "INF";
std::string const BigInt::NEG_INF_STR = "-INF";


// private functions:

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
    if (data.size())
	return static_cast<SZ>(std::log2(data.back())) + shift_sz * (data.size() - 1);
    throw_exception<std::domain_error>("BigInt log2 called on " + to_string(*this));
}

// floor(abs(this)/2^n) % 2
B BigInt::getBit(UI n) const {
    if (n / shift_sz < data.size()) {
	return ((data[n / shift_sz] >> (n % shift_sz)) & 1u) == 1u;
    } else {
	return false;
    }
}

// floor(sqrt(this))
BigInt BigInt::sqrt() const {
    // f(x) = this-x^2
    // f'(x) = -2x
    if (!*this)
	return *this;
    if (neg())
	throw_exception<std::domain_error>("BigInt sqrt called on " + to_string(*this));
    if (inf())
	return *this;
    SZ const digits = log2()/2 + 1;
    BigInt rt = BigInt(1) << static_cast<SL>(digits-1);
    if (rt*rt > *this)
	throw_exception<std::runtime_error>("Unexpected runtime error when calculating BigInt.sqrt on " + to_string(*this) + ": initial guess (" + to_string(rt) + ") too high with square " + to_string(rt*rt));
    for (SZ i = digits-1; i--; ) {
	small_t const bit = small_t(1) << i%shift_sz;
	rt.data[i/shift_sz] += bit;
	if (rt*rt > *this)
	    rt.data[i/shift_sz] -= bit;
    }
    return rt;
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
    if (!*this)
	return 0;
    return neg() ? -1 : 1;
}

// swap
void BigInt::swap(BigInt& other) {
    std::swap(infinite,other.infinite);
    std::swap(negative,other.negative);
    std::swap(data,other.data);
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
    addOrSubtract(o, AddOrSubtract::Add);
    return *this;
}
BigInt& BigInt::operator-=(BigInt const& o) {
    addOrSubtract(o, AddOrSubtract::Subtract);
    return *this;
}
void BigInt::addOrSubtract(BigInt const& other, AddOrSubtract action) {
    B matchesSign = (negative == other.negative) == (action == AddOrSubtract::Add);
    if (inf()) {
	if (other.inf() && !matchesSign) {
	    throw_exception<std::domain_error>(static_cast<std::string>(*this) +
					       (action == AddOrSubtract::Add ? " + " : " -") +
					       static_cast<std::string>(other));
	}
	return;
    }
    if (other.inf()) {
	*this = other;
	if (action == AddOrSubtract::Subtract) {
	    negateMe();
	}
	return;
    }
    if (matchesSign) {
	// same sign -> addition -> carry indicates add 1
	B carry = false;
	SZ i = 0;
	if (data.size() < other.data.size()) {
	    data.resize(other.data.size(), small_t(0));
	}
	for (; i < other.data.size(); ++i) {
	    if (carry) {
		carry = 0 == ++data[i];
	    }
	    small_t res = data[i] + other.data[i];
	    carry |= res < data[i];
	    data[i] = res;
	}
	while (carry) {
	    if (i == data.size()) {
		data.emplace_back(1);
		break;
	    } else {
		carry = 0 == ++data[i++];
	    }
	}
    } else {
	if (this->abs() < other.abs()) {
	    BigInt withOtherFirst(other);
	    withOtherFirst.addOrSubtract(*this, action);
	    if (action == AddOrSubtract::Subtract) {
		// other - this -> this - other
		withOtherFirst.negateMe();
	    }
	    swap(withOtherFirst);
	    return;
	}
	
	// different signs -> subtraction -> carry indicates subtract 1
	B carry = false;
	SZ i = 0;
	for (; i < other.data.size(); ++i) {
	    if (carry) {
		carry = 0 == data[i]--;
	    }
	    carry |= data[i] < other.data[i];
	    data[i] -= other.data[i];
	}
	if (carry) {
	    while (0 == data[i++]--); // intentional ;
	}
	reduce();
    }
}
BigInt& BigInt::operator*=(BigInt const& other) {
    if (!*this)
	return *this;
    if (!other)
	return *this = other;
    
    if (inf() || other.inf()) {
	infinite = Infinite::True;
	if (other.neg())
	    negateMe();
	data.clear();
	return *this;
    }
    
    if (other.isPowerOf2()) {
	if (other.neg())
	    negateMe();
	return *this <<= static_cast<SL>(other.log2());
    }
    
    if (isPowerOf2()) {
	auto pow = log2();
	bool const wasNeg = neg();
	*this = other;
	if (wasNeg)
	    negateMe();
	return *this <<= static_cast<SL>(pow);
    }
    
    if (other.neg())
	negateMe();
    BigInt result{Infinite::False, negative};
    auto& rdata = result.data;
    auto const& odata = other.data;
    SZ size = data.size();
    SZ osize = odata.size();
	    
    rdata.resize(size + osize - 1, 0);

    big_t current = 0;
    big_t carry = 0;
    for (SZ new_d = 0; new_d < osize; ++new_d) {
	current = carry;
	carry = 0;
	for (SZ i = 0; i < size && i <= new_d; ++i) {
	    current += big_t(data[i]) * big_t(odata[new_d-i]);
	    carry += current >> shift_sz;
	    current &= low_digits;
	}
	rdata[new_d] = static_cast<small_t>(current);
    }
    for (SZ new_d = osize; new_d + 1 < size + osize; ++new_d) {
	current = carry;
	carry = 0;
	for (SZ i = new_d - osize + 1; i < size && i <= new_d; ++i) {
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
void BigInt::divOrRem(BigInt const& other, DivOrRem action) {
    if (!other) {
	throw_exception<std::domain_error>("div/rem by zero");
    }
    if (inf() && other.inf()) {
	throw_exception<std::domain_error>("±Inf div/rem ±Inf");
    }
    if (!*this) {
	return;
    }
    if (other.neg() && action == DivOrRem::Div) {
	// For the rest of this function, we ignore the sign of other.
	// *this has the final sign (with a final reduce() at the end, if it becomes -0).
	negateMe();
    }
    if (inf()) {
	if (action == DivOrRem::Rem) {
	    throw_exception<std::domain_error>("±Inf rem x");
	}
	return;
    }
    if (other.inf()) {
	if (action == DivOrRem::Div) {
	    *this = BigInt{};
	}
	return;
    }
    
    if (other.isPowerOf2()) {
	if (action == DivOrRem::Div) {
	    *this >>= static_cast<SL>(other.log2());
	} else {
	    SZ bits = other.log2();
	    SZ data_sz = (bits + shift_sz - 1) / shift_sz;
	    if (data.size() > data_sz)
		data.resize(data_sz);
	    SZ small_bits = bits % shift_sz;
	    if (small_bits > 0 && data.size() == data_sz)
		data.back() &= (small_t(1) << small_bits) - small_t(1);
	    reduce();
	}
	return;
    }
    
    if (other.data.size() == 1) {
	big_t value = 0;
	small_t divisor = other.data[0];
	for (SZ i = data.size(); i-- > 0; ) {
	    value = (value << shift_sz) | data[i];
	    if (action == DivOrRem::Div) {
		data[i] = static_cast<small_t>(value / divisor);
	    }
	    value %= divisor;
	}
	if (action == DivOrRem::Rem) {
	    B const is_neg = neg();
	    *this = value;
	    if (is_neg) {
		negateMe();
	    }
	}
	reduce();
	return;
    }

    SZ shift;
    {
	auto l2 = log2();
	auto ol2 = other.log2();
	if (l2 < ol2) {
	    if (action == DivOrRem::Div) {
		*this = BigInt{};
	    }
	    return;
	}
	shift = l2 - ol2;
    }
    B resultNegative = neg();
    absMe();

    BigInt divResult;
    divResult.data.resize(shift / shift_sz + 1);
    
    BigInt times = other.abs();
    times <<= static_cast<SL>(shift);
    for (++shift; shift--; times >>= 1) {
	if (*this >= times) {
	    *this -= times;
	    divResult.data[shift / shift_sz] |= small_t(1) << (shift % shift_sz);
	}
    }
    if (action == DivOrRem::Div) {
	swap(divResult);
    }
    negative = resultNegative ? Negative::True : Negative::False;
    reduce();
}
// always rounds towards 0
BigInt& BigInt::operator/=(BigInt const& other) {
    divOrRem(other, DivOrRem::Div);
    return *this;
}
// always has the same sign as *this (unless becomes 0)
BigInt& BigInt::operator%=(BigInt const& other) {
    divOrRem(other, DivOrRem::Rem);
    return *this;
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
    if (negative)
	is.get();

    BigInt result;
    B init = false;
    while (is) {
	C const next = static_cast<C>(is.peek());
	if (std::isdigit(next)) {
	    init = true;
	    result *= 10;
	    result += next - '0';
	    is.get();
	} else {
	    break;
	}
    }

    if (!init)
	return Failure("No digits encountered for BigInt");

    if (negative && result)
	result.negateMe();
    
    resetter.ignore();
    return result;
}

template<> std::string Class<BigInt>::name() {
    return "BigInt";
}

template<> std::string Class<BigInt>::format() {
    return "[-]?[0-9]{1+}";
}
