#pragma once

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "MyHashUtil.hpp"
#include "MyHashVector.hpp"
#include "Optional.hpp"
#include "SafeMath.hpp"
#include "ScopedSetValue.hpp"
#include "Str.hpp"
#include "StringUtils.hpp"
#include "TypeUtils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>
#include <utility>

class BigInt;
namespace std {
    template<> struct make_unsigned<BigInt> { using type = BigInt; };
    template<> struct is_signed<BigInt> { static constexpr bool value = true; };
    template<> struct is_integral<BigInt> { static constexpr bool value = true; };
}

class BigInt {
  private:
    using big_t = UL;
    using small_t = UI;
    // the operations we do on data necessitate that the following expressions
    // do not overflow, for data x, y, and z:
    // x+y+1, x+y*z
    static_assert(sizeof(big_t) >= 2*sizeof(small_t),
		  "Numeric types have invalid sizes for BigInt");
    static constexpr small_t max_small_t = std::numeric_limits<small_t>::max();
    static constexpr big_t low_digits = max_small_t;
    static constexpr big_t mod = low_digits + 1;
    static constexpr small_t shift_sz = 8 * sizeof(small_t);

    // type-safe booleans
    enum class Infinite : bool { False = false, True = true};
    enum class Negative : bool { False = false, True = true};
    BigInt(Infinite inf, Negative neg);
    
    // member variables;
    // [least significant digit, ..., most significant digit]
    std::vector<small_t> data;
    Infinite infinite;
    Negative negative;
    
    // std::hash for BigInt
    friend struct std::hash<BigInt>;
    friend struct Class<BigInt>;
    
    class Impl;
    friend class Impl;

    // deletes extra 0's from the data
    void reduce();
    
  public:
    static std::string const INF_STR;
    static std::string const NEG_INF_STR;
    
    // constructors
    BigInt();
    BigInt(std::string const& s);
    template<class T, class = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, BigInt>>>
    BigInt(T const& t);

    // infinite constants
    static BigInt const& Inf();
    static BigInt const& NegInf();

    bool inf() const;
    bool neg() const;
    int cmp() const;
    // |x| < |y|
    friend bool lessThanInAbs(BigInt const& x, BigInt const& y);

    BigInt& absMe();
    BigInt& negateMe();

    friend void swap(BigInt&, BigInt&);
    void swap(BigInt &);

    // returns if this is +- a power of 2
    bool isPowerOf2() const;

    // returns floor(log2(abs(this))) = ⌊log2(|this|)⌋
    // throws if this == 0
    std::size_t log2() const;

    // returns ceil(log2(abs(this))) = ⌈log2(|this|)⌉
    // throws if this == 0
    std::size_t ceilLog2() const;

    // returns floor(abs(this)/2^n) % 2
    bool getBit(SZ n) const;

    // returns floor(sqrt(this))
    BigInt sqrt() const;

    // type conversion operators
    explicit operator bool() const;
    explicit operator std::string() const;
    template<class T> Optional<T> convert() const;

    // comparison operators
    friend bool operator<(BigInt const& x, BigInt const& y);
    friend bool operator>(BigInt const& x, BigInt const& y);
    friend bool operator<=(BigInt const& x, BigInt const& y);
    friend bool operator>=(BigInt const& x, BigInt const& y);
    friend bool operator==(BigInt const& x, BigInt const& y);
    friend bool operator!=(BigInt const& x, BigInt const& y);

    // arithmetic operators
    BigInt abs() const;
    BigInt operator-() const;

    BigInt& operator++();
    BigInt& operator--();
    
    BigInt& operator<<=(long);
    BigInt& operator>>=(long);
    
    BigInt operator<<(long) const;
    BigInt operator>>(long) const;

    friend BigInt operator+(BigInt x, BigInt const& y);
    BigInt& operator+=(BigInt const& o);

    friend BigInt operator-(BigInt x, BigInt const& y);
    BigInt& operator-=(BigInt const& o);
    
    friend BigInt operator*(BigInt x, BigInt const& y);
    BigInt& operator*=(BigInt const& o);

    // / and /= round towards 0
    friend BigInt operator/(BigInt x, BigInt const& y);
    BigInt& operator/=(BigInt const& o);
    friend BigInt divideRoundAwayFrom0(BigInt x, BigInt const& y);

    friend BigInt operator%(BigInt x, BigInt const& y);
    BigInt& operator%=(BigInt const& o);
    
    BigInt operator^(std::size_t exp) const;
    BigInt& operator^=(std::size_t expt);
};
MAKE_HASHABLE(BigInt,x,x.infinite,x.negative,x.data);

template<> std::ostream& Class<BigInt>::print(std::ostream& oss, BigInt const& bi);
template<> Optional<BigInt> Class<BigInt>::parse(std::istream& is);
template<> Str Class<BigInt>::name();
template<> Str Class<BigInt>::format();

template<>
class std::numeric_limits<BigInt> {
  public:
    static constexpr bool is_specialized = true;
    static BigInt min() noexcept { return -infinity(); }
    static BigInt max() noexcept { return infinity(); }
    static BigInt lowest() noexcept { return min(); }
    static constexpr int  digits = std::numeric_limits<int>::max();
    static constexpr int  digits10 = std::numeric_limits<int>::max();
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static BigInt epsilon() noexcept { return 1; }
    static BigInt round_error() noexcept { return 1; }
    
    static constexpr int  min_exponent = 0;
    static constexpr int  min_exponent10 = 0;
    static constexpr int  max_exponent = 0;
    static constexpr int  max_exponent10 = 0;
    
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static BigInt infinity() noexcept { return BigInt::Inf(); }
    static BigInt quiet_NaN() noexcept { return 0; }
    static BigInt signaling_NaN() noexcept { return 0; }
    static BigInt denorm_min() noexcept { return 0; }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};



// template impls

template<class T, class>
BigInt::BigInt(T const& t)
    : infinite{std::numeric_limits<T>::has_infinity &&
	(t == std::numeric_limits<T>::infinity() ||
	 (std::is_signed<T>::value &&
	  t == -std::numeric_limits<T>::infinity())) ?
	Infinite::True : Infinite::False}
    , negative{(std::numeric_limits<T>::is_signed && t < static_cast<T>(0)) ?
	      Negative::True : Negative::False} {
    using u_T = std::make_unsigned_t<T>;
    u_T const u_0 = 0;
    
    constexpr bool too_small = sizeof(u_T) <= sizeof(small_t);
    
    u_T value = my_abs(t);
    if constexpr (too_small) {
	if (value > u_0) {
	    data = {static_cast<small_t>(value)};
	}
    } else {
	for ( ; value > u_0; value >>= shift_sz) {
	    data.push_back(value & low_digits);
	}
    }
}

template<class T> Optional<T> BigInt::convert() const {
    static_assert(is_pure<T>,"conversion can only be to non-qualified types");
    auto failure = [this]() -> Failure {
	return {Class<T>::name() +
		" cannot hold BigInt value " +
		std::string(*this)};
    };
    
    if (neg() && std::is_unsigned_v<T> && static_cast<bool>(*this))
	return failure();
    
    if (inf()) {
	if (!std::numeric_limits<T>::has_infinity)
	    return failure();
	T infinity = std::numeric_limits<T>::infinity();
	return neg() ? -infinity : infinity;
    }

    ScopedSetValue allow_fail(SafeMath_failFast,false);
    SafeMath<T> safe_result(T(0));
    for (SZ i = data.size(); i-- > 0; ) {
	safe_result *= mod;
	if (neg())
	    safe_result -= data[i];
	else
	    safe_result += data[i];
    }
    if (!safe_result.ok())
	return failure();
    return *safe_result;
}

